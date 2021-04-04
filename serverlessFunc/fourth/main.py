import json
import boto3
from boto3.dynamodb.conditions import Key
import requests

TELE_TOKEN='1752485400:AAGy7fN6dZw95qmtiKND6ahW68wJY0fNPac'
URL = "https://api.telegram.org/bot{}/".format(TELE_TOKEN)

def lambda_handler(event, context):
    dynamodb = boto3.resource('dynamodb')
    id = event['id']
    table = dynamodb.Table('ccproject')      
    
    deviceStatus = {
    'id': "1",
    'status': 'active',
    }
    tableStatus = dynamodb.Table('ccproject')
    tableStatus.put_item(Item=deviceStatus)
    # for val in resp['Item']['access']:
    #     print(val)
    #     final_text = "Notification: " + "device " + id + " has detected a fall. Please provide immediate assistance!"
    #     url = URL + "sendMessage?text={}&chat_id={}".format(final_text, val)
    #     requests.post(url)
    #     deviceStatus = {
    #     'id': "1",
    #     'status': 'fall',
    #     }
    #     tableStatus = dynamodb.Table('ccproject')
    #     tableStatus.put_item(Item=deviceStatus)
    return {
        'statusCode': 200,
        'body': json.dumps('Hello from Lambda!')
    }

# def lambda_handler(event, context):
#     # TODO implement
#     return {
#         'statusCode': 200,
#         'body': json.dumps('Hello from Lambda!')
#     }
