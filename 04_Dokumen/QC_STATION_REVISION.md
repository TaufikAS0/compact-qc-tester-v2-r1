# QC station revision1.9.0 -2026-10-15

User authorizes implementation of seven follow-up ideas, self-check and OTA after pushing baseline. Baseline2b5af1a/1.8.0 pushed/up-to-date first. Feature branch only; no merge or separate general serial-web repository.

| ID | Deliverable | Evidence boundary |
| --- | --- | --- |
| ST-01 | Per-M test recipe: two Modbus tags, analog tolerance and references, serial fixture baud/packets/timeout, bounded output step timeout. Persist independently of profile mask/calibration. | Own schema/CRC/revision/readback; no implicit I2C/output/radio writes when loading. Recipe revision/CRC follows report/QC code. |
| ST-02 | Readiness panel for connection/profile/config/calibration/sensors/required fixtures and data freshness. | Readiness is not QC PASS; no automatic test execution, no ACK-as-identity. |
| ST-03 | Clickable schematic connection map, verified V2_R1 pins, status and links to relevant tests. | Explicitly schematic, not PCB component coordinates. Preserve TOP/BOTTOM revision labels. |
| ST-04 | Guided LED/DO ON/OFF observations, enabled channels only, bounded timeout, Stop/session/disconnect cancellation. | Each stage explicitly started/observed; no inferred physical PASS, no auto-resume/automatic energizing after reload. |
| ST-05 | Serial search/history/retest comparison; merge reports from device browser bundles into durable loopback laptop archive. | Append-only immutable reports; do not overwrite failed earlier results, no hardware/COM open on import. |
| ST-06 | Printable report/label with QR carrying traceable identity/code/reference, plus prominent real status. | INCOMPLETE/FAIL never branded PASS. QC code/QR is trace reference, not signature/certificate. QR generated offline; no remote report upload. |
| ST-07 | Bounded mini charts and freshness/communication indicators for current/temp/RH. | No fake interpolation or samples for stale/invalid data; stability is diagnostic, not accuracy acceptance. |

Maintain existing mask profiles/NVS/calibration/EEPROM/WiFi/TPL/polarity/legacy partitions. Minor1.8.0->1.9.0. Source and exact build/OTA/runtime proof logged in CURRENT/VALIDATION. Guided output tested in mocks without actual ON unless fitted fixture is verified. Browser checks remain subject to current computer-use policy, no bypass.
