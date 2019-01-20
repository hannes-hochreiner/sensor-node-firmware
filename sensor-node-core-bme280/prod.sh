make clean
read -p "Please enter the encryption key (32 hex digits, no prefix): " key
read -p "Please enter the sensor id (4 hex digits, no prefix: e.g. 'AA55'): " sensor_id
DEBUG=0 KEY0=0x${key:24:8} KEY1=0x${key:16:8} KEY2=0x${key:8:8} KEY3=0x${key:0:8} SENSOR_ID=0x${sensor_id} make -e
scp ../common/oo_prod.cfg hc1:/tmp
scp build/sensor-node-core-bme280.elf hc1:/tmp/sensor-node.elf
ssh hc1 "sudo openocd -f /tmp/oo_prod.cfg"
ssh hc1 'rm /tmp/sensor-node.elf'
ssh hc1 'rm /tmp/oo_prod.cfg'
make clean
