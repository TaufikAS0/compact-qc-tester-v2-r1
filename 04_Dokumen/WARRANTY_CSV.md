# QC CSV and Warranty inventory CSV1.12.0

Owner supplied inventory CSV2026-10-15; only its schema/product labels are used. Sample serials, people and dates are not production defaults. Owner confirmed PCB belongs ONLY in the full QC CSV, not Warranty.

## Full QC CSV

Existing `/export/qc.csv`, UTF8 BOM/RFC4180/formula protection, first ten Longhi columns and every existing detailed column remain in the same order. New `pcbNumber` is appended LAST. No QC result/status changes. Snapshot/JSON/print/browser archives retain the session PCB; old immutable archived CSV files are not rewritten.

Summary ->No PCB ->review/save. Text max64 UTF8 bytes, no controls/newlines; leading zeroes preserved. This is metadata, not EEPROM identity or a test verdict. POST `/api/qc/pcb` requires token,confirm=1,sessionId,profileRevision,expectedPcbNumber and pcbNumber; stale source409, missing/invalid400, auth403. Empty explicit value clears metadata. Saves/readbacks the existing QC progress blob;507 means not durably saved. Other tests/RTC stay unchanged, outputs OFF. New session clears PCB; same-build resume restores it. Unsaved browser draft blocks exports/reports until saved.

## Warranty CSV

`GET /export/warranty.csv?qcDate=YYYY-MM-DD&productionDate=YYYY-MM-DD&qcPic=...`

Exact10 headers, no extra PCB column:

```csv
serialNumber,productName,productType,productionDate,qcStatus,qcDate,qcPic,status,deliveredAt,deliveredTo
```

One data row, UTF8 WITHOUT BOM (matches example), CRLF/RFC4180 and formula-safe metadata. Filename is ONLY the actual device serial plus `.csv`, e.g. `TMC88F1559CA28C.csv`. Browser validates exact header and matching serial before saving; firmware validates safe serial filename.

| Field | Source |
|---|---|
| serialNumber | Actual device hardware serial |
| productName | Telemetric Module Compact |
| productType | Monitoring and Control |
| productionDate | Operator production-date input, required |
| qcStatus | Actual current overall QC; export allowed only PASS |
| qcDate | Operator QC-date input, required |
| qcPic | Operator/PIC input, required, max64 bytes |
| status | INVENTORY |
| deliveredAt / deliveredTo | Empty; delivery not asserted |

Missing/invalid dates/PIC or production after QC ->400; unselected/incomplete/failed/busy QC or commanded ON outputs ->409. PASS is never forced from the sample row. Read-only export does not store invented PIC/date or contact/import into Warranty web. Current RTC PASS scope remains production quick Write/Readback; daily/battery sampling are separate per RTC_PORTAL_CONTRACT.

The user changed OTA target to M1 at192.168.1.44, a DIFFERENT serial from the earlier device. Fresh0PASS baseline was archived before upgrade; do not copy old unit results or fabricate PASS merely to test a successful Warranty download. Read CURRENT/VALIDATION for actual deployment and validation limits.

## Checkpoint upgrade

Normal restore still requires exact build/unit/profile/preset/calibration. For deliberate upgrade ONLY source1.12.0 accepts explicit `upgradeFromBuild=QC_BUILD=1.11.0|2026-10-15T08:16:55Z`, matching stored source/clock and no RTC run. Original/previous source provenance retained; missing PCB stays empty. This exception does not apply to importing another unit or arbitrary edited JSON. The fresh1.9.1 replacement has no selected QC evidence to migrate; start its own M1 session after upload.
