# sailfish_timing_system

## Prep

Run the following cmds for prep

```bash
sudo apt-get update -y && apt-get upgrade -y
sudo apt-get install midori unclutter -y
```

## Install
Run the following cmds to install

```bash
git clone https://github.com/2ffs2nns/sailfish_timing_system.git
```

Then modify `/etc/xdg/lxsession/LXDE-pi/autostart`

```bash
# /etc/xdg/lxsession/LXDE-pi/autostart
@lxpanel --profile LXDE-pi
@pcmanfm --desktop --profile LXDE-pi
@xset -dpms s off

@unclutter -idle 0
@lxterminal -e python3 /home/vjo/sailfish_timing_system/app.py
@lxterminal -e python3 /home/vjo/sailfish_timing_system/start_button.py
@/usr/bin/python /home/vjo/sailfish_timing_system/run_midori.py
```

Then, install `RaspAP` by following the (guide)[https://raspap.com/#quick]

```bash
curl -sL https://install.raspap.com | bash
```

Then run `shutdown -r now` to reboot.