# EPANET v2.2 Patches

This repository is a fork of [v2.2 of owa-epanet](https://github.com/OpenWaterAnalytics/EPANET/tree/v2.2) and is used by [epanet-js](https://github.com/modelcreate/epanet-js) to provide patches for bugs in the v2.2 API.

The patches focus solely on correcting API functionality to match the original behavior described in v2.2.

## Fix EN_settankdata for elevation with SI units #594

- PR: [#594](https://github.com/OpenWaterAnalytics/EPANET/pull/594)
- Issue: [#593](https://github.com/OpenWaterAnalytics/EPANET/issues/593)
- Patch: [6619dbe](https://github.com/modelcreate/EPANET/commit/6619dbe7596b667afb5fc34eb1d2e455398835d8)

**Problem:** When using metric units, the `EN_settankdata` toolkit function failed to correctly set the tank's bottom elevation. The elevation value provided in meters was assigned internally without the necessary conversion to feet (EPANET's internal unit), and subsequent level calculations were also incorrect. Retrieving the elevation afterwards using `EN_getnodevalue` returned an improperly converted value (e.g., 800 meters became 243.84 meters).

**Patch Fix:** The patch corrects the `EN_settankdata` function by ensuring the input elevation `elev` is properly divided by the elevation unit conversion factor (`Ucf[ELEV]`) before being stored internally. It also adjusts the calculations for absolute water levels (`H0`, `Hmin`, `Hmax`) to correctly combine the input elevation and levels _before_ converting the sum to internal units. This ensures the elevation and levels are stored correctly regardless of the project's unit system.

```c
// Example showing the API calls from the issue report (Metric Units)
EN_Project ph;
int index;
double required_elevation = 800.0; // Meters
double retrieved_elevation;

// ... (Project created, Metric units set, tank node added) ...

// Set tank data including elevation using EN_settankdata
// (Levels/diameter simplified here for clarity on elevation issue)
EN_settankdata(ph, index, required_elevation, /*initlvl*/ 1.0, /*minlvl*/ 1.0, /*maxlvl*/ 2.0, /*diam*/ 10.0, /*minvol*/ 0.0, "");

// Retrieve the elevation value
EN_getnodevalue(ph, index, EN_ELEVATION, &retrieved_elevation);

// Before Patch: retrieved_elevation = 243.840000 (Incorrect - 800 / 3.2808...)
// After Patch:  retrieved_elevation = 800.000000 (Correct)
```

## Fix Unconnected nodes garbled error message #609

- PR: [#594](https://github.com/OpenWaterAnalytics/EPANET/pull/609)
- Issue: [#593](https://github.com/OpenWaterAnalytics/EPANET/issues/608)
- Patch: [5b0a3f6](https://github.com/modelcreate/EPANET/commit/5b0a3f678e8c29fa47e8af70c8c1ea27f392273d)
