# PD presets and confirmation - v1.7.0

User request2026-10-14: easier9/12/20V selection and slider with confirmation; investigate16/17/18V.

Facts: STUSB4500 supports fixed USB PD sink negotiation and no PPS (ST datasheet DS12499 rev8 section3.3.2 and ST employee PPS answer). Sink PDO50mV encoding does not create a continuously adjustable source. Exact voltage plus sufficient source current must match; no match can fallback to5V.16/17/18V require the same advertised fixed source PDO. Observed9/12/20 success is user evidence, not a newly measured contract. M1 screenshot shows PD ignored, not a negotiation failure. Adapter capabilities and electrical VBUS for this revision remain unknown.

Sources: https://www.st.com/resource/en/datasheet/stusb4500.pdf and https://community.st.com/interface-and-connectivity-ics-52/how-do-i-configure-the-stusb4500-as-a-pps-sink-47487

Controls:5/9/12/15/20V presets; slider5..20V step50mV; numeric fine entry retained. All only edit draft. Review shows exact voltage/current, source snapshot match/unknown/insufficient-current warning and5V fallback possibility. Confirm sends once after fresh state/context check. Cancel, changed draft/session/profile/source snapshot, disconnect and Stop invalidate review. No delayed confirmed request after Stop. /api/pd now requires confirm=1 (400 without it, before stop/I2C); token/profile/work-owner guards remain. This intent field is not an authentication or human-proof mechanism. No PPS/QC alternate protocol emulation.

M1 PD stays OFF; enabling requires explicit profile configuration. Snapshot is advisory and may be stale; negotiation remains authoritative. Protocol contract is not measured VBUS. No STUSB NVM write, pin/NVS/partition/WiFi changes. Continuous voltages need PPS-capable hardware and source, or a suitably rated downstream adjustable regulator; not implemented on this board.

Assessment: presets/slider8/10, limited by adapter fixed PDOs; PPS through STUSB4500 firmware0/10, unsupported hardware. Unknown: adapter model/caps, actual VBUS, load/cable ratings. No fabricated physical PASS.

Validation/build/OTA/publication state: see latest CURRENT and VALIDATION. Tests cover draft-only, cancellation, invalid step, snapshot unknown/mismatch/current, context expiry, double click/rejection, Stop during refresh and M1 gates. Live voltage commands are not required for UI acceptance and were not sent by Codex.
