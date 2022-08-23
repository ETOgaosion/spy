echo "before online, cpu online list"
cat /sys/devices/system/cpu/online
echo 1 | sudo tee /sys/devices/system/cpu/cpu2/online
echo "after online, cpu online list"
cat /sys/devices/system/cpu/online
echo "======= building ========"
make all
make test
echo "======= testing ========"
echo "offline cpu:"
echo 0 | sudo tee /sys/devices/system/cpu/cpu2/online
echo "output result:"
dmesg | grep spy