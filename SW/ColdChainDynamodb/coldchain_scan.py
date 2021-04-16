import boto3
from boto3.dynamodb.conditions import Key, Attr

# Get the service resource.
dynamodb = boto3.resource('dynamodb')

cold_chain_table = dynamodb.Table("COLD_CHAIN")

print(cold_chain_table.creation_date_time)

response = cold_chain_table.scan(
    FilterExpression = Attr('TravelId').eq('4') & Attr('SensorId').eq('0000-0000-0000-0005')
)


print(len(response['Items']))
print(response['Items'])
