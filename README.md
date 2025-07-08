# HammerIdFix

**Metamod plugin for fixing broken Hammer entity IDs on CS2 servers.**

---

## Overview

`HammerIdFix` is a native Metamod plugin for Counter-Strike 2 that patches internal memory to resolve issues with broken or inconsistent Hammer editor entity IDs (`m_iHammerID`).

* Works on both **Linux** and **Windows**
* Automatic memory patching at startup
* No configuration required
* Supports dedicated servers and development environments
* Designed for integration into plugin pipelines

---

## Features

* Resolves invalid or corrupt Hammer editor entity IDs (`m_iHammerID`)
* Fully native C++ implementation
* Compatible with latest CS2 builds (as of 2025)
* Minimal overhead – patches memory once at plugin load
* Safe memory unprotection and patching logic
* Separate builds for Linux and Windows

---

## File Output

This plugin does not generate any output or logs under normal operation.  
It silently applies the required memory fix during initialization.

For debugging, you may enable `CORE_INFO` or similar logging in the source.

---

## Configuration

None required.

Just place the compiled plugin into:

```
/addons/HammerIdFix/bin/linuxsteamrt64/  (Linux)
/addons/HammerIdFix/bin/win64/           (Windows)
```

and it will apply the patch automatically at startup.

---

## Compatibility

* CS2 Dedicated Server (latest)
* Works with any map or plugin setup
* No effect on CS:GO or CS2 client (only server-side)

---

## Technical Notes

This plugin uses low-level memory patching to overwrite hardcoded instruction sequences or faulty logic tied to `m_iHammerID`.

> If the target memory region changes in a future CS2 update, the plugin may need to be rebuilt with updated offsets or signatures.

---

## License

[GPLv3](https://www.gnu.org/licenses/gpl-3.0.en.html)

---

## Author

**Slynx / [funplay.pro](https://funplay.pro/)**  
Made with ❤️ for Hammer users and plugin developers.
