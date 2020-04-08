2bwm.substitute()
{
    local reslist devlist libdir includedir bindir cmd i perl_version \
    tmpbindir vendor_perl \
    cmdlist='dirname basename cat ls mv sudo cp chmod ln chown rm touch
    head mkdir perl mktemp shred egrep sed less date env bash
    xdotool sleep gtk-demo figlet convert xsetroot display'

    declare -A Devlist=(
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
    ws_alpha=7
    ws_beta=8
    ws_gamma=9
    ws_delta=5
    ws_epsilon=6
    timer='0.5'
    \builtin source <($cat<<-SUB

2bwm.verify()
{
    \builtin \trap "2bwm.verify.delocate" SIGHUP SIGTERM SIGINT
    local i
    declare -a Tests=(
    2bwm.verify1   
    2bwm.verify2
    2bwm.verify3
    2bwm.verify4
    2bwm.verify5
    2bwm.verify6
    )
    declare -a Tfuns=(2bwm.figlet 2bwm.setrootwindow)
    2bwm.verify.delocate()
    {
        \builtin unset -f \${Tests[@]}
        \builtin unset -f \${Tfuns[@]}
        \builtin unset Tests Tfuns
        \builtin \trap - SIGHUP SIGTERM SIGINT
        set +o xtrace
    }
    2bwm.setrootwindow()
    {
        local text=\${1}
        local tmpfile=/tmp/\${RANDOM}
        if [[ -z \${text} ]];then 
            $xsetroot -default
            return
        fi
        \builtin printf "text 15,15 \"" > \${tmpfile}
        $figlet -f block \${text} >> \${tmpfile}
        \builtin printf "\"" >> \${tmpfile}
        $convert -size 160x160 xc:black -fill white -draw "@\${tmpfile}" \${tmpfile}.png
        $display -window root \${tmpfile}.png
        $rm -f \${tmpfile} \${tmpfile}.png
    }
    2bwm.verify6()
    {
        # Verify cross ws_alpha selection/move
        # change ws_alpha
        $xdotool sleep ${timer} key super+${ws_alpha}
        2bwm.setrootwindow ${ws_alpha}
        $xdotool sleep ${timer} key super+w
        2bwm.figlet ${ws_alpha}
        $xdotool sleep ${timer} key super+g 
        # change ws_alpha
        $xdotool sleep ${timer} key super+${ws_beta}
        2bwm.setrootwindow ${ws_beta}
        $xdotool sleep ${timer} key super+w
        2bwm.figlet ${ws_beta}
        $xdotool sleep ${timer} key super+ctrl+g 
        # change ws_alpha
        $xdotool sleep ${timer} key super+${ws_gamma}
        2bwm.setrootwindow ${ws_gamma}
        $xdotool sleep ${timer} key super+w
        2bwm.figlet ${ws_gamma}
        $xdotool sleep ${timer} key super+n 
        # change ws_alpha
        $xdotool sleep ${timer} key super+${ws_epsilon}
        2bwm.setrootwindow ${ws_epsilon}
        $xdotool sleep ${timer} key super+w
        2bwm.figlet ${ws_epsilon}
        $xdotool sleep ${timer} key super+ctrl+n
        $xdotool sleep ${timer} key super+o
        $xdotool sleep ${timer} key super+${ws_gamma}
        2bwm.setrootwindow ${ws_gamma}
        $xdotool sleep ${timer} key super+o
        $xdotool sleep ${timer} key super+${ws_beta}
        2bwm.setrootwindow ${ws_beta}
        $xdotool sleep ${timer} key super+o
        $xdotool sleep ${timer} key super+${ws_alpha}
        2bwm.setrootwindow ${ws_alpha}
        $xdotool sleep ${timer} key super+o
        # move to ws_delta
        $xdotool sleep ${timer} key super+shift+${ws_delta}
        $xdotool sleep ${timer} key super+${ws_delta}
        2bwm.setrootwindow ${ws_delta}
        # selectall
        $xdotool sleep ${timer} key super+shift+o
        #  deletewin
        $xdotool sleep ${timer} key super+p
        2bwm.setrootwindow
    } 
    2bwm.verify5()
    {
        # Verify hide/unhide
        # change ws_alpha
        $xdotool sleep ${timer} key super+${ws_alpha}
        2bwm.setrootwindow ${ws_alpha}
        # newwin, teleporting
        $xdotool sleep ${timer} key super+w
        2bwm.figlet 1
        $xdotool sleep ${timer} key --delay 500 super+g 
        $xdotool sleep ${timer} key super+w
        2bwm.figlet 2
        $xdotool sleep ${timer} key --delay 500 super+ctrl+g 
        $xdotool sleep ${timer} key super+w
        2bwm.figlet 3
        $xdotool sleep ${timer} key --delay 500 super+n 
        $xdotool sleep ${timer} key super+w
        2bwm.figlet 4
        $xdotool sleep ${timer} key --delay 500 super+ctrl+n
        # selectwin
        $xdotool sleep ${timer} key super+o
        $xdotool sleep ${timer} key super+Tab
        # selectwin
        $xdotool sleep ${timer} key super+o
        # hide
        $xdotool sleep ${timer} key super+i
        # unhide
        $xdotool sleep ${timer} key super+shift+i
        # selectall
        $xdotool sleep ${timer} key super+shift+o
        #  deletewin
        $xdotool sleep ${timer} key super+p
        2bwm.setrootwindow
    } 
    2bwm.verify4()
    {
        # Verify sidebyside
        # change ws_alpha
        $xdotool sleep ${timer} key super+${ws_alpha}
        2bwm.setrootwindow ${ws_alpha}
        # newwin, teleporting
        $xdotool sleep ${timer} key super+w
        2bwm.figlet 1
        $xdotool sleep ${timer} key super+w
        2bwm.figlet 2
        $xdotool sleep ${timer} key super+w
        2bwm.figlet 3
        $xdotool sleep ${timer} key super+w
        2bwm.figlet 4
        # selectall
        $xdotool sleep ${timer} key super+shift+o
        # resize a8
        $xdotool sleep ${timer} key super+ctrl+8
        # sidebyside
        $xdotool sleep ${timer} key super+s
        # focuswin,focuslocationwin
        $xdotool sleep ${timer} key super+Tab
        # focuswin,focuslocationwin
        $xdotool sleep ${timer} key super+ctrl+Tab
        # focuswin,focuslocationwin
        $xdotool sleep ${timer} key super+Tab
        # focuswin,focuslocationwin
        $xdotool sleep ${timer} key super+ctrl+Tab
        # swapwin
        $xdotool sleep ${timer} key super+shift+s
        # focuswin,focuslocationwin
        $xdotool sleep ${timer} key super+ctrl+Tab
        # focuswin,focuslocationwin
        $xdotool sleep ${timer} key super+ctrl+Tab
        # focuswin,focuslocationwin
        $xdotool sleep ${timer} key super+ctrl+Tab
        # selectall
        $xdotool sleep ${timer} key super+shift+o
        #  deletewin
        $xdotool sleep ${timer} key super+p
        2bwm.setrootwindow
    } 
    2bwm.verify3()
    {
        # Verify ws_alpha focus 
        # change ws_alpha
        $xdotool sleep ${timer} key super+${ws_alpha}
        2bwm.setrootwindow ${ws_alpha}
        # newwin, teleporting
        $xdotool sleep ${timer} key super+w
        2bwm.figlet 1
        $xdotool sleep ${timer} key super+g 
        $xdotool sleep ${timer} key super+w
        2bwm.figlet 2
        $xdotool sleep ${timer} key super+ctrl+g 
        $xdotool sleep ${timer} key super+w
        2bwm.figlet 3
        $xdotool sleep ${timer} key super+n 
        $xdotool sleep ${timer} key super+w
        2bwm.figlet 4
        $xdotool sleep ${timer} key super+ctrl+n 
        # focuswin,focuslocationwin
        $xdotool sleep ${timer} key super+Tab
        # changews_alpha
        $xdotool sleep ${timer} key super+${ws_beta}
        2bwm.setrootwindow ${ws_beta}
        # newwin, teleporting
        $xdotool sleep ${timer} key super+w
        2bwm.figlet 5
        $xdotool sleep ${timer} key super+g 
        $xdotool sleep ${timer} key super+w
        2bwm.figlet 6
        $xdotool sleep ${timer} key super+ctrl+g 
        $xdotool sleep ${timer} key super+w
        2bwm.figlet 7
        $xdotool sleep ${timer} key super+n 
        $xdotool sleep ${timer} key super+w
        2bwm.figlet 8
        $xdotool sleep ${timer} key super+ctrl+n 
        # focuswin,focuslocationwin
        $xdotool sleep ${timer} key super+Tab
        # changews_alpha
        $xdotool sleep ${timer} key super+${ws_alpha}
        2bwm.setrootwindow ${ws_alpha}
        # focuswin,focuslocationwin
        $xdotool sleep ${timer} key super+Tab
        # changews_alpha
        $xdotool sleep ${timer} key super+${ws_beta}
        2bwm.setrootwindow ${ws_beta}
        # focuswin,focuslocationwin
        $xdotool sleep ${timer} key super+ctrl+Tab
        # changews_alpha
        $xdotool sleep ${timer} key super+${ws_alpha}
        2bwm.setrootwindow ${ws_alpha}
        # focuswin,focuslocationwin
        $xdotool sleep ${timer} key super+ctrl+Tab
        # changews_alpha
        $xdotool sleep ${timer} key super+${ws_beta}
        2bwm.setrootwindow ${ws_beta}
        # selectall
        $xdotool sleep ${timer} key super+shift+o
        #  deletewin
        $xdotool sleep ${timer} key super+p
        # changews_alpha
        $xdotool sleep ${timer} key super+${ws_alpha}
        2bwm.setrootwindow ${ws_alpha}
        # selectall
        $xdotool sleep ${timer} key super+shift+o
        #  deletewin
        $xdotool sleep ${timer} key super+p
        2bwm.setrootwindow
    }
    2bwm.figlet()
    {
        $xdotool sleep ${timer} type --clearmodifiers "PS1=''" 
        $xdotool key --clearmodifiers --delay 0 Return Ctrl+l 
        $xdotool sleep ${timer} type --clearmodifiers "$figlet -f block \${@}"
        $xdotool key --delay 0 Return
    }
    2bwm.verify1()
    {
        # verify newwin, teleport,send/change ws_alpha
        $xdotool sleep ${timer} key super+${ws_alpha}
        2bwm.setrootwindow ${ws_alpha}
        # newwin, teleporting
        $xdotool sleep ${timer} key super+w
        2bwm.figlet 1
        $xdotool sleep ${timer} key super+g
        $xdotool sleep ${timer} key super+w
        2bwm.figlet 2
        $xdotool sleep ${timer} key super+ctrl+g 
        $xdotool sleep ${timer} key super+w
        2bwm.figlet 3
        $xdotool sleep ${timer} key super+n 
        $xdotool sleep ${timer} key super+w
        2bwm.figlet 4
        $xdotool sleep ${timer} key super+ctrl+n 
        # focuswin,focuslocationwin
        $xdotool sleep ${timer} key super+Tab
        $xdotool sleep ${timer} key super+ctrl+Tab
        $xdotool sleep ${timer} key super+Tab
        $xdotool sleep ${timer} key super+ctrl+Tab
        # sendtows_alpha
        $xdotool sleep ${timer} key super+shift+${ws_beta}
        # changetows_alpha
        $xdotool sleep ${timer} key super+${ws_beta}
        2bwm.setrootwindow ${ws_beta}
        # changetows_alpha
        $xdotool sleep ${timer} key super+${ws_alpha}
        2bwm.setrootwindow ${ws_alpha}
        # multi-select
        $xdotool sleep ${timer} key super+shift+o
        # sendtows_alpha
        $xdotool sleep ${timer} key super+shift+${ws_beta}
        # changetows_alpha
        $xdotool sleep ${timer} key super+${ws_beta}
        2bwm.setrootwindow ${ws_beta}
        # multi-select
        $xdotool sleep ${timer} key super+shift+o
        #  deletewin
        $xdotool sleep ${timer} key super+p
        $xsetroot -default
    }
    2bwm.verify2()
    {
        # verify X11 app
        $xdotool sleep ${timer} key super+${ws_alpha}
        2bwm.setrootwindow ${ws_alpha}
        # gtk-demo
        $gtk_demo &
        $xdotool sleep ${timer} key Up Return
        # movestep
        $xdotool sleep ${timer} key --delay 1000 super+l
        $xdotool sleep ${timer} key alt+f
        $xdotool sleep ${timer} key Return
        $xdotool sleep ${timer} key Return
        $xdotool sleep ${timer} key super+Tab
        #  deletewin
        $xdotool sleep ${timer} key super+p
        $xdotool sleep ${timer} key super+p
        2bwm.setrootwindow
    }
    for i in \${Tests[@]};do
        \${i}
    done
    2bwm.verify.delocate
}
SUB
)
}
2bwm.substitute
builtin unset -f 2bwm.substitute
