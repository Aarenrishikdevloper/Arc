# Arc

Arc is a keyboard-first terminal file shell with a built-in file browser, live preview pane, search, sorting, and marked-item workflow.

Fast navigation. Predictable TUI behavior. A focused file workflow without leaving the terminal.

---

# Why Arc

Arc combines a lightweight shell and a file-oriented TUI into a single binary. Browse the filesystem, inspect files, batch-mark items, search, sort, and manage files from one interface.

The current version focuses on stable terminal behavior, clean rendering, reliable raw-mode handling, and a professional preview experience.

---

# Features

- Built-in shell with common filesystem commands
- Interactive preview mode with live file preview
- Keyboard-first navigation
- Fuzzy search
- Sorting by filename and modification date
- Batch marking workflow
- Text file viewer with scrolling
- Safe preview rendering for terminal output
- Automatic terminal restoration on exit

---

# Preview Mode

- Framed file browser
- Live preview pane
- Status and help footer
- Fast keyboard navigation

---

# Shell Mode

Interactive shell with built-in filesystem commands.

---

# Built-in Commands

| Command | Description |
|----------|-------------|
| pwd | Print current working directory |
| cd [path] | Change directory |
| ls [-a] [path...] | List directory contents |
| cat filename | Display file contents |
| touch [-a] [-m] [-t STAMP] file... | Create files or update timestamps |
| mkdir [-p] [-v] [-m MODE] dir... | Create directories |
| rm [-f] [-i] [-I] [-r] [-R] [-v] path... | Remove files or directories |
| clear | Clear the terminal |
| preview | Launch Preview Mode |
| exit | Exit Arc |

Quoted paths and escaped spaces are supported.

mkdir "folder with spaces"
cat "notes.txt"
cd "/tmp/demo folder"
---

# Preview Controls

## Navigation

| Key | Action |
|-----|--------|
| ↑ / ↓ | Move cursor |
| → / Enter | Open directory |
| ← / Backspace | Go to parent directory |
| Enter (on text file) | Open viewer |
| q | Exit Preview Mode |

---

## Search & Sorting

| Key | Action |
|-----|--------|
| : | Enter search mode |
| Esc | Clear search |
| Enter | Keep filtered results |
| a | Sort by name (ascending) |
| A | Sort by name (descending) |
| d | Sort by date (ascending) |
| D | Sort by date (descending) |
| s | Refresh directory |

---

## Marked Mode

| Key | Action |
|-----|--------|
| Space | Toggle mark |
| m | Mark all visible items |
| u | Clear all marks |
| i | Invert selection |
| x | Delete marked items |
| Esc | Exit marked mode |

Deletion confirmation uses a single key:

- y — Confirm
- n — Cancel
- Esc — Cancel

---

## Text Viewer

| Key | Action |
|-----|--------|
| ↑ / ↓ | Scroll |
| q / Esc | Close viewer |

---

# Building

## Configure

cmake -S . -B build
## Build

cmake --build build
## Run

./build/arc
---

# Recent Improvements

- Redesigned preview frame using continuous box-drawing characters
- Cleaner status and help footer
- Improved batch marking workflow
- Better viewer exit behavior
- Fixed rendering near terminal edges
- Improved command handling across different terminal emulators
- More reliable raw-mode restoration

---

# Project Structure

```
.
├── headers/
├── helps/
├── src/
├── CMakeLists.txt
└── README.md
```

---

# Notes

- Preview mode is optimized for text files.
- Binary files are automatically detected and displayed as non-previewable.
- Terminal state is restored on normal exit and common termination paths.
- Designed for keyboard-driven navigation and efficient file management.