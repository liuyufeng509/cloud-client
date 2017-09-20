#/bin/bash
if [ `apt-cache show cdos-desktop-cloud | grep Version | awk -F  ' ' '{print $2}' ` == `dpkg -l | grep cdos-desktop-cloud | awk -F ' ' '{print $3}'` ];
then
	echo 0;
else
	echo 1;
fi
