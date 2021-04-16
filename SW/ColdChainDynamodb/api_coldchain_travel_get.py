import requests

API_ENDPOINT = 'https://ij60i8kpw0.execute-api.us-east-1.amazonaws.com/prod/api/coldchain/'

travel = str(input("Travel Id:"))
GET_TRAVEL_INFO_ENDPOINT = f'{API_ENDPOINT}{travel}'
print(GET_TRAVEL_INFO_ENDPOINT)

response = requests.get(url = GET_TRAVEL_INFO_ENDPOINT)
print(response.content)