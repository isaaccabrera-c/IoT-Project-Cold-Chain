import requests
import boto3
import uuid
from datetime import datetime
from decimal import Decimal
import time

API_ENDPOINT = "https://ij60i8kpw0.execute-api.us-east-1.amazonaws.com/prod/api/coldchain"

# Generate hardcoded data

sensors = [
  "48",
  "4F"
]
travel = "000C"
data = []
for i in range(len(sensors)):
  for j in range(0, 2):
    now = datetime.now()
    print(f'{sensors[i]} - {str(now)}')
    data.append(
      {
        'S': sensors[i],
        'C': "1E00",
        'T': "0008068b",
        'V': travel,
      }
    )
    time.sleep(1)

print(data)

r = requests.post(url = API_ENDPOINT, json = data)
print(r)
