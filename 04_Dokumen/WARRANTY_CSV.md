Owner decision2026-10-15: PASS-only rule superseded. FAIL may be exported with qcStatus=FAIL; never relabel failure as PASS. INCOMPLETE remains blocked. Current compatible upgrade ONLY exact1.13.0/build11:18:57Z->1.13.1; earlier checkpoint exceptions below are historical.

# QC CSV and Warranty inventory CSV1.13.1

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
| qcStatus | Actual current overall QC; export allowed PASS or FAIL, preserving the actual status |
| qcDate | Operator QC-date input, required |
| qcPic | Operator/PIC input, required, max64 bytes |
| status | INVENTORY |
| deliveredAt / deliveredTo | Empty; delivery not asserted |

Missing/invalid dates/PIC or production after QC ->400; unselected/INCOMPLETE/busy QC or commanded ON outputs ->409. PASS is never forced from the sample row. Read-only export does not store invented PIC/date or contact/import into Warranty web. Current RTC PASS scope remains production quick Write/Readback; daily/battery sampling are separate per RTC_PORTAL_CONTRACT.

The user changed OTA target to M1 at192.168.1.44, a DIFFERENT serial from the earlier device. Fresh0PASS baseline was archived before upgrade; do not copy old unit results or fabricate PASS merely to test a successful Warranty download. Read CURRENT/VALIDATION for actual deployment and validation limits.

## Checkpoint upgrade

Normal restore still requires exact build/unit/profile/preset/calibration. For deliberate upgrade ONLY source1.12.0 accepts explicit `upgradeFromBuild=QC_BUILD=1.11.0|2026-10-15T08:16:55Z`, matching stored source/clock and no RTC run. Original/previous source provenance retained; missing PCB stays empty. This exception does not apply to importing another unit or arbitrary edited JSON. The fresh1.9.1 replacement has no selected QC evidence to migrate; start its own M1 session after upload.

## Installed acceptance2026-10-15

Firmware on replacement M1 at192.168.1.44 upgraded1.9.1 ->1.12.0, actual build2026-10-15T08:59:41Z /15:59:41 WIB. App-only OTA HTTP200; actual serialTMC88F1559CA28C/build/runtime MD5 matched, profiles and inactive calibration preserved, no erase. Feature source d1ef3d2 was pushed BEFORE OTA. App1530208 bytes SHA256cf55d345afa0215f6daf1ac1c5a2b5ac6985276ad6a9c1cebed8da2c47a31635, MD5 7a17c29e84860296b63d37805c2ccd72. No firmware edits after final build began.

Actual Operator browser created this unit's own M1 rev1/sessionB6361F85. Temporary PCB000TEST-PCB saved/read back, full QC CSV preserved all previous columns in order with pcbNumber LAST, and the same session/PCB survived identical-binary OTA reboot and explicit resume. Temporary PCB then cleared and checkpoint saved/read back clean; final PCB empty awaiting actual production number. Actual browser download C:/Users/taufi/Downloads/Compact_QC.csv verified this serial and empty final PCB. Screenshot/evidence remain ignored under build/warranty-144-*.

Actual Warranty endpoint correctly rejected incomplete QC409, missing PIC400, invalid date400 and production-after-QC400. Exact10 header/serial-only filename/successful PASS row are software-tested; live successful Warranty download/import remains unverified because this replacement is only1PASS(AHT communication)/0FAIL/33pending/4ignored/overallINCOMPLETE. No manufactured PASS, no old unit34PASS copied. RTC clockRevision0/OSF active, no quick RTC/time write on this unit. No output ON, EEPROM write, PD request or radioApply. Build/Node UI/Python14/portal packaging2/date+PCB assertions PASS. main unchanged35733ba; feature only, no merge.
