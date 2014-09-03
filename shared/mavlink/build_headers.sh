BACKUP=$PYTHONPATH
export PYTHONPATH=$BACKUP:../../mavlink/
rm -R onboard/
cp ../../mavlink/message_definitions/v1.0/common.xml .
python -m pymavlink.generator.mavgen UniAdapter.xml --lang C -o onboard/
export PYTHONPATH=$BACKUP