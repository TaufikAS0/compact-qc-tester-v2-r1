"""QC UART protocol v1. Binary framing shared with serial_qc_protocol.h."""
import struct
import time
import binascii

HEADER = struct.Struct('<2sBBIHH')
REQUEST, RESPONSE, ACK, ACK_REPLY = 1, 2, 3, 4

def payload(run, seq):
    size = 16 << (seq % 4)
    return bytes([0, 255, 85, 170] + [((run >> ((i % 4) * 8)) ^ (seq * 37) ^ (i * 73)) & 255 for i in range(4, size)])

def encode(kind, run, seq):
    data = payload(run, seq) if kind in (REQUEST, RESPONSE) else b''
    if kind == RESPONSE:
        data = bytes(b ^ 0xa5 for b in data)
    raw = HEADER.pack(b'QC', 1, kind, run, seq, len(data)) + data
    return raw + struct.pack('<H', binascii.crc_hqx(raw, 0xffff))

def decode(raw):
    if len(raw) < 14:
        raise ValueError('Short frame')
    magic, version, kind, run, seq, n = HEADER.unpack(raw[:12])
    if magic != b'QC' or version != 1 or n > 128 or len(raw) != 14 + n:
        raise ValueError('Invalid frame header/length')
    if binascii.crc_hqx(raw[:-2], 0xffff) != struct.unpack('<H', raw[-2:])[0]:
        raise ValueError('CRC mismatch')
    return kind, run, seq, raw[12:-2]

def read_exact(port, size, deadline, cancel):
    out = bytearray()
    while len(out) < size:
        if cancel.is_set():
            raise InterruptedError('Cancelled by technician')
        if time.monotonic() >= deadline:
            raise TimeoutError('RS232 response timeout')
        out.extend(port.read(size - len(out)))
    return bytes(out)

def receive(port, deadline, cancel):
    head = read_exact(port, 12, deadline, cancel)
    magic, version, _, _, _, n = HEADER.unpack(head)
    if magic != b'QC' or version != 1 or n > 128:
        raise ValueError('Unexpected bytes/header; check port, fixture and baud')
    raw = head + read_exact(port, n + 2, deadline, cancel)
    decode(raw)
    return raw

def exercise(port, run, packets, timeout, cancel, progress):
    """No retries: any mismatch/timeout fails this QC run."""
    latencies = []
    for seq in range(packets):
        started = time.monotonic()
        deadline = started + timeout
        request = encode(REQUEST, run, seq)
        if port.write(request) != len(request):
            raise IOError('Short serial write')
        reply = receive(port, deadline, cancel)
        if reply != encode(RESPONSE, run, seq):
            raise ValueError('Response type/run/sequence/pattern mismatch (echo cannot PASS)')
        ack = encode(ACK, run, seq)
        if port.write(ack) != len(ack):
            raise IOError('Short serial ACK write')
        if receive(port, deadline, cancel) != encode(ACK_REPLY, run, seq):
            raise ValueError('ACK reply mismatch')
        latencies.append((time.monotonic() - started) * 1000)
        progress(seq + 1, latencies[-1])
    return latencies
