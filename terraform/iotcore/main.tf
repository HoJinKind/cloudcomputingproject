provider "aws" {
  region = "ap-southeast-1"
}


# resource "aws_iot_thing" "example" {
#   name = "device1"

#   attributes = {
#     First = "device1"
#   }
# }

resource "aws_iot_policy" "device1policy" {
  name   = "device1policy"
  policy = <<EOT
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Effect": "Allow",
      "Action": "iot:Connect",
      "Resource": "arn:aws:iot:ap-southeast-1:194467213443:client/device1"
    },
    {
      "Effect": "Allow",
      "Action": "iot:Subscribe",
      "Resource": "arn:aws:iot:ap-southeast-1:194467213443:topicfilter/esp32/sub/data"
    },
    {
      "Effect": "Allow",
      "Action": "iot:Subscribe",
      "Resource": "arn:aws:iot:ap-southeast-1:194467213443:topicfilter/esp32/sub/url"
    },
    {
      "Effect": "Allow",
      "Action": "iot:Receive",
      "Resource": "arn:aws:iot:ap-southeast-1:194467213443:topic/esp32/sub/url"
    },
    {
      "Effect": "Allow",
      "Action": "iot:Receive",
      "Resource": "arn:aws:iot:ap-southeast-1:194467213443:topic/esp32/sub/data"
    },
    {
      "Effect": "Allow",
      "Action": "iot:Publish",
      "Resource": "arn:aws:iot:ap-southeast-1:194467213443:topic/esp32/pub/data"
    },
    {
      "Effect": "Allow",
      "Action": "iot:Publish",
      "Resource": "arn:aws:iot:ap-southeast-1:194467213443:topic/esp32/pub/url"
    }
  ]
}
EOT
}

