# cros-copyme

Given a string from stdin, copy it into the clipboard of the Chrome OS, automatically.

Yeah I can't find any workarounds on the internet since Chrome OS 81 and it has been broken as of v88. So I write it on my own. Issues & PRs are welcome.

Dependency: GTK 3.

```
apt install libgtk-3-dev
```

Sample Usage:

```
make
echo "Hello world" | ./copyme
```

## TODO

* The current implementation adds a 100 ms delay in hope of correctly persist the clipboard content. This ugly hack is unfortunately not bullet-proof. I know nothing of the internal mechanism of [Sommelier](https://chromium.googlesource.com/chromiumos/platform2/+/HEAD/vm_tools/sommelier). Should get rid of this.
* The application should not show in the task bar. This should be possible, i.e., fcitx's tray window, but I cannot reproduce.

## Related threads

* [Chrome -> Wayland clipboard broken](https://www.reddit.com/r/Crostini/comments/cgpsfa/chrome_wayland_clipboard_broken/).
* [Copying to clipboard using xclip/xsel no longer working in Crostini on ChromeOS 81
](https://www.reddit.com/r/Crostini/comments/fxs2i4/copying_to_clipboard_using_xclipxsel_no_longer/).
