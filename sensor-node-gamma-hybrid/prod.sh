make clean
read -p "Please enter the encryption key: " key
DEBUG=0 KEY0=0x${key:0:8} KEY1=0x${key:8:8} KEY2=0x${key:16:8} KEY3=0x${key:24:8} make -e
scp ../common/oo_prod.cfg hc1:/tmp
scp build/sensor-node-gamma-hybrid.elf hc1:/tmp
ssh hc1 "sudo openocd -f /tmp/oo_prod.cfg"
ssh hc1 'rm /tmp/sensor-node-gamma-hybrid.elf'
ssh hc1 'rm /tmp/oo_prod.cfg'
make clean
