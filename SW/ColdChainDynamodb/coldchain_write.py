import boto3
import uuid
from datetime import datetime
from decimal import Decimal
import time

# Get the service resource.
dynamodb = boto3.resource('dynamodb')

cold_chain_table = dynamodb.Table("COLD_CHAIN")

print(cold_chain_table.creation_date_time)

sensors = [
  "0000-0000-0000-0001",
  "0000-0000-0000-0002",
  "0000-0000-0000-0003",
  "0000-0000-0000-0004",
  "0000-0000-0000-0005",
  "0000-0000-0000-0006",
  "0000-0000-0000-0007",
  "0000-0000-0000-0008"
]

travel = "3"

for i in range(len(sensors)):
  for j in range(0, 4):
    now = datetime.now()
    print(f'{sensors[i]} - {str(now)}')
    cold_chain_table.put_item(
      Item={
        'UniqueId': str(uuid.uuid1()),
        'SensorId': sensors[i],
        'Temperature': Decimal('45.2'),
        'Timestamp': str(now),
        'TravelId': travel,
      }
    )
    time.sleep(5)
