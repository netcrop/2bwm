![2bwm](https://raw.github.com/venam/2bwm/master/2bWM.png)

## 2bwm:
A fast floating Window Manager using XCB. 
This repo is based on a fork from ![2bwm](https://github.com/venam/2bwm).
With a new data structure, more features have been developed.

## New Features:
* select multiple windows across workspaces or:
* select all windows across workspaces and:
    *  move them to a new workspace.
    *  teleport them.
    *  resize into A4,A8 paper size.
	*  hide and unhide.
    *  delete them all together.
* arrange all windows in a workspace into two side-by-side circular lists.
    * change focus on windows for each list.
    * move focus window from one side to the other.
* individual focused window on each workspace.

![Side by side](misc/sidebyside.gif?raw=true "")

## Dependencies
xcb-util-wm
xcb-util-xrm
xcb-util-keysyms
## Install/Uninstall
```
# The following commands require sudo.
> cd 2bwm
> source 2bwm.sh
> 2bwm.install
# build archlinux package
> 2bwm.build
```
## For developers
![delete window 1](misc/data.png?raw=true "")
![delete window 2](misc/data2.png?raw=true "")

The purpose of test cases are for verifying each existing features and should be executed and passed.
Each pull request with new features should come together with new test cases. 
```
# Install test script.
> 2bwm.fun2script 2bwm.verify
# Execute 2bwm.verify from a application launcher e.g: [dmenu] [rofi]
# Test cases will occupy first and second workspaces on your display.
```
## Releases:
**arch** is the latest developing **branch**.
each **release** tag will be created from this **branch**.
e.g. **v1.0** is the first stable **release** tag form **arch** **branch**.

## Reporting a issues and pull requests
github.com/netcrop/2bwm/pulls

## License

[ISC License](LICENSE?raw=true "")
