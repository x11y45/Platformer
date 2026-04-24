<div align="center">

```
██████╗ ██╗      █████╗ ████████╗███████╗ ██████╗ ██████╗ ███╗   ███╗███████╗██████╗
██╔══██╗██║     ██╔══██╗╚══██╔══╝██╔════╝██╔═══██╗██╔══██╗████╗ ████║██╔════╝██╔══██╗
██████╔╝██║     ███████║   ██║   █████╗  ██║   ██║██████╔╝██╔████╔██║█████╗  ██████╔╝
██╔═══╝ ██║     ██╔══██║   ██║   ██╔══╝  ██║   ██║██╔══██╗██║╚██╔╝██║██╔══╝  ██╔══██╗
██║     ███████╗██║  ██║   ██║   ██║     ╚██████╔╝██║  ██║██║ ╚═╝ ██║███████╗██║  ██║
╚═╝     ╚══════╝╚═╝  ╚═╝   ╚═╝   ╚═╝      ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝╚══════╝╚═╝  ╚═╝
```

[![Repo](https://img.shields.io/badge/⚔_Source_Code-x11y45%2FPlatformer-8B0000?style=for-the-badge&logo=github&logoColor=white)](https://github.com/x11y45/Platformer)
[![Release](https://img.shields.io/badge/🐧_Linux_Release-Download-8B0000?style=for-the-badge)](https://github.com/x11y45/Platformer/releases)
[![C++](https://img.shields.io/badge/C%2B%2B-17-darkred?style=for-the-badge&logo=cplusplus&logoColor=white)](https://en.cppreference.com/w/cpp/17)
[![SFML](https://img.shields.io/badge/SFML-2.6.2-darkred?style=for-the-badge)](https://www.sfml-dev.org/)

</div>

---

> **📌 Note for the reviewer** — This GitHub repository is the official submission for the OOP course project. The full source code is here. The pre-compiled release targets **Linux only** — for other platforms, see [Build from source](#-build-from-source).

---

## 🎮 Controls

<table>
<tr><th>Key</th><th>Action</th></tr>
<tr><td><kbd>←</kbd> <kbd>A</kbd></td><td>Move left</td></tr>
<tr><td><kbd>→</kbd> <kbd>D</kbd></td><td>Move right</td></tr>
<tr><td><kbd>↑</kbd> <kbd>W</kbd> <kbd>Space</kbd></td><td>Jump &nbsp;·&nbsp; press twice for double jump</td></tr>
<tr><td><kbd>↓</kbd> <kbd>S</kbd></td><td>Crouch</td></tr>
<tr><td><kbd>X</kbd> <kbd>K</kbd></td><td>Attack</td></tr>
<tr><td><kbd>C</kbd></td><td>Heal <em>(costs 20 karma · restores 40 HP · must be idle on the ground)</em></td></tr>
<tr><td><kbd>Esc</kbd></td><td>Pause</td></tr>
</table>

---

## 🚀 Download & Run

### 🐧 Linux — Pre-built release

```bash
# Download from → https://github.com/x11y45/Platformer/releases
unzip Platformer-Linux-x86_64.zip
cd Platformer-Linux-x86_64
chmod +x Platformer
./Platformer
```

### 🔨 Build from source

> Works on Linux, Windows and macOS. CMake downloads SFML automatically — no manual setup needed.

```bash
git clone https://github.com/x11y45/Platformer.git
cd Platformer
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

./build/Platformer              # Linux / macOS
.\build\Release\Platformer.exe  # Windows
```

<details>
<summary><b>Linux — extra system dependencies</b></summary>
<br>

```bash
sudo apt-get install -y \
  libx11-dev libxrandr-dev libxcursor-dev libxi-dev \
  libudev-dev libflac-dev libvorbis-dev libopenal-dev libgl1-mesa-dev
```

</details>

---

## 🎨 Credits

| Asset | Author | License |
|:---|:---|:---|
| Town *(tileset & sprites)* | [ansimuz](https://ansimuz.itch.io/) | CC0 |
| Magic Cliffs *(backgrounds)* | [ansimuz](https://ansimuz.itch.io/) | CC0 |
| FrostGuardian *(boss)* | [Chierit](https://chierit.itch.io/) | CC0 |
| Music & SFX | CC0 sources | CC0 |