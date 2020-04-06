2bwm.substitute()
{
    local reslist devlist libdir includedir bindir cmd i perl_version \
    tmpbindir vendor_perl \
    cmdlist='dirname basename cat ls mv sudo cp chmod ln chown rm touch
    head mkdir perl mktemp shred egrep sed less date env bash'

    declare -A Devlist=(
        [xdotool]=xdotool
        [sleep]=sleep
        [gtk-demo]=gtk-demo
        [makepkg]=makepkg
    )
    cmdlist="${Devlist[@]} $cmdlist"
    for cmd in $cmdlist;do
        i=($(\builtin type -afp $cmd 2>/dev/null))
        if [[ -z $i ]];then
            if [[ -z ${Devlist[$cmd]} ]];then
                reslist+=" $cmd"
            else
                devlist+=" $cmd"
            fi
        fi
        \builtin eval "local ${cmd//-/_}=${i:-:}"
    done
    [[ -z $reslist ]] ||\
    { 
        \builtin printf "%s\n" \
        "$FUNCNAME says: ( $reslist ) These Required Commands are missing."
        return
    }
    [[ -z $devlist ]] ||\
    \builtin printf "%s\n" \
    "$FUNCNAME says: ( $devlist ) These Optional Commands for further development."

    perl_version="$($perl -e 'print $^V')"
    vendor_perl=/usr/share/perl5/vendor_perl/
    libdir=/usr/local/lib
    includedir=/usr/local/include/
    bindir=/usr/local/bin/
    tmpbindir=/var/tmp/
    \builtin source <($cat<<-SUB

2bwm.verify()
{
    # These test cases should no be executed from a terminal.
    # They will occupy workspace 1 and 2.
    declare -a Tests=(
    2bwm.verify1   
    2bwm.verify2
    2bwm.verify3
    2bwm.verify4
    2bwm.verify5
    2bwm.verify6
    )
    2bwm.verify6()
    {
        # Verify cross workspace selection/move
        $sleep 1
        # change workspace
        $xdotool key super+1
        $sleep 1
        $xdotool key super+w
        $sleep 1
        $xdotool key 1
        $sleep 1
        $xdotool key super+g 
        $sleep 1
        # change workspace
        $xdotool key super+2
        $sleep 1
        $xdotool key super+w
        $sleep 1
        $xdotool key 2
        $sleep 1
        $xdotool key super+ctrl+g 
        $sleep 1
        # change workspace
        $xdotool key super+3
        $sleep 1
        $xdotool key super+w
        $sleep 1
        $xdotool key 3
        $sleep 1
        $xdotool key super+n 
        $sleep 1
        # change workspace
        $xdotool key super+4
        $sleep 1
        $xdotool key super+w
        $sleep 1
        $xdotool key 4
        $xdotool key super+ctrl+n
        $sleep 1
        $xdotool key super+o
        $sleep 1
        $xdotool key super+3
        $sleep 1
        $xdotool key super+o
        $sleep 1
        $xdotool key super+2
        $sleep 1
        $xdotool key super+o
        $sleep 1
        $xdotool key super+1
        $sleep 1
        $xdotool key super+o
        $sleep 1
        # move
        $xdotool key super+shift+5
        $sleep 1
        $xdotool key super+5
        # selectall
        $xdotool key super+shift+o
        $sleep 1
        #  deletewin
        $xdotool key super+p
        $sleep 2
    } 
    2bwm.verify5()
    {
        $sleep 1
        # Verify hide/unhide
        # change workspace
        $xdotool key super+1
        $sleep 1
        # newwin, teleporting
        $xdotool key super+w
        $sleep 1
        $xdotool key 1
        $sleep 1
        $xdotool key --delay 500 super+g 
        $xdotool key super+w
        $sleep 1
        $xdotool key 2
        $sleep 1
        $xdotool key --delay 500 super+ctrl+g 
        $xdotool key super+w
        $sleep 1
        $xdotool key 3
        $sleep 1
        $xdotool key --delay 500 super+n 
        $xdotool key super+w
        $sleep 1
        $xdotool key 4
        $xdotool key --delay 500 super+ctrl+n
        $sleep 2
        # selectwin
        $xdotool key super+o
        $sleep 1
        $xdotool key super+Tab
        # selectwin
        $xdotool key super+o
        $sleep 1
        # hide
        $xdotool key super+i
        $sleep 1
        # unhide
        $xdotool key super+shift+i
        $sleep 1
        # selectall
        $xdotool key super+shift+o
        $sleep 1
        #  deletewin
        $xdotool key super+p
        $sleep 2
    } 
    2bwm.verify4()
    {
        $sleep 1
        # Verify sidebyside
        # change workspace
        $xdotool key super+1
        $sleep 1
        # newwin, teleporting
        $xdotool key super+w
        $sleep 1
        $xdotool key 1
        $sleep 1
        $xdotool key super+w
        $sleep 1
        $xdotool key 2
        $sleep 1
        $xdotool key super+w
        $sleep 1
        $xdotool key 3
        $sleep 1
        $xdotool key super+w
        $sleep 1
        $xdotool key 4
        $sleep 2
        # selectall
        $xdotool key super+shift+o
        $sleep 1
        # resize a8
        $xdotool key super+ctrl+8
        $sleep 1
        # sidebyside
        $xdotool key super+s
        $sleep 1
        # focuswin,focuslocationwin
        $xdotool key super+Tab
        $sleep 1
        # focuswin,focuslocationwin
        $xdotool key super+ctrl+Tab
        $sleep 1
        # focuswin,focuslocationwin
        $xdotool key super+Tab
        $sleep 1
        # focuswin,focuslocationwin
        $xdotool key super+ctrl+Tab
        $sleep 1
        # swapwin
        $xdotool key super+shift+s
        $sleep 1
        # focuswin,focuslocationwin
        $xdotool key super+ctrl+Tab
        $sleep 1
        # focuswin,focuslocationwin
        $xdotool key super+ctrl+Tab
        $sleep 1
        # focuswin,focuslocationwin
        $xdotool key super+ctrl+Tab
        $sleep 1
        # selectall
        $xdotool key super+shift+o
        $sleep 1
        #  deletewin
        $xdotool key super+p
        $sleep 2
    } 
    2bwm.verify3()
    {
        $sleep 1
        # Verify workspace focus 
        # change workspace
        $xdotool key super+1
        $sleep 1
        # newwin, teleporting
        $xdotool key super+w 1
        $xdotool key --delay 500 super+g 
        $xdotool key super+w 2
        $xdotool key --delay 500 super+ctrl+g 
        $xdotool key super+w 3
        $xdotool key --delay 500 super+n 
        $xdotool key super+w 4
        $xdotool key --delay 500 super+ctrl+n 
        # focuswin,focuslocationwin
        $xdotool key super+Tab
        $sleep 2
        # changeworkspace
        $xdotool key super+2
        $sleep 1
        # newwin, teleporting
        $xdotool key super+w 5
        $xdotool key --delay 500 super+g 
        $xdotool key super+w 6
        $xdotool key --delay 500 super+ctrl+g 
        $xdotool key super+w 7
        $xdotool key --delay 500 super+n 
        $xdotool key super+w 8
        $xdotool key --delay 500 super+ctrl+n 
        $sleep 2
        # focuswin,focuslocationwin
        $xdotool key super+Tab
        $sleep 1
        # changeworkspace
        $xdotool key super+1
        $sleep 1
        # focuswin,focuslocationwin
        $xdotool key super+Tab
        $sleep 1
        # changeworkspace
        $xdotool key super+2
        $sleep 1
        # focuswin,focuslocationwin
        $xdotool key super+ctrl+Tab
        $sleep 1
        # changeworkspace
        $xdotool key super+1
        $sleep 1
        # focuswin,focuslocationwin
        $xdotool key super+ctrl+Tab
        $sleep 1
        # changeworkspace
        $xdotool key super+2
        $sleep 1
        # selectall
        $xdotool key super+shift+o
        $sleep 1
        #  deletewin
        $xdotool key super+p
        $sleep 1
      # changeworkspace
        $xdotool key super+1
        $sleep 1
        # selectall
        $xdotool key super+shift+o
        $sleep 1
        #  deletewin
        $xdotool key super+p
        $sleep 2
    }
    2bwm.verify1()
    {
        $sleep 1
        # verify newwin, teleport,send/change workspace
        $xdotool key super+1
        $sleep 1
        # newwin, teleporting
        $xdotool key super+w 1
        $xdotool key --delay 500 super+g 
        $xdotool key super+w 2
        $xdotool key --delay 500 super+ctrl+g 
        $xdotool key super+w 3
        $xdotool key --delay 500 super+n 
        $xdotool key super+w 4
        $xdotool key --delay 500 super+ctrl+n 
        $sleep 2
        # focuswin,focuslocationwin
        $xdotool key super+Tab
        $sleep 1
        $xdotool key super+ctrl+Tab
        $sleep 1
        $xdotool key super+Tab
        $sleep 1
        $xdotool key super+ctrl+Tab
        $sleep 2
        # sendtoworkspace
        $xdotool key super+shift+2
        $sleep 1
        # changetoworkspace
        $xdotool key super+2
        $sleep 1
        # changetoworkspace
        $xdotool key super+1
        $sleep 1
        # multi-select
        $xdotool key super+shift+o
        $sleep 1
        # sendtoworkspace
        $xdotool key super+shift+2
        $sleep 1
        # changetoworkspace
        $xdotool key super+2
        $sleep 1
        # multi-select
        $xdotool key super+shift+o
        $sleep 1
        #  deletewin
        $xdotool key super+p
        $sleep 2
    }
    2bwm.verify2()
    {
        # verify X11 app
        $sleep 1
        $xdotool key super+1
        $sleep 1
        # gtk-demo
        $gtk_demo &
        $sleep 1
        $xdotool key Up Return
        $sleep 1
        # movestep
        $xdotool key --delay 1000 super+l
        $sleep 1
        $xdotool key alt+f
        $sleep 1
        $xdotool key Return
        $sleep 1
        $xdotool key Return
        $sleep 1
        $xdotool key super+Tab
        $sleep 1
        #  deletewin
        $xdotool key super+p
        $sleep 1
        $xdotool key super+p
        $sleep 1
    }
    local fun
    set -o xtrace
    for fun in \${Tests[@]};do
    $cat <<-2BWMVERIFY > ${tmpbindir}/\${fun}
#!$env $bash
\$(\builtin declare -f \${fun})
\${fun} '\$@'
2BWMVERIFY
    \builtin unset -f \${fun}
    $chmod u=rwx ${tmpbindir}/\${fun}
    ${tmpbindir}/\${fun}
    $rm -f ${tmpbindir}/\${fun}
    done
    set +o xtrace
}
SUB
)
}
2bwm.substitute
builtin unset -f 2bwm.substitute
