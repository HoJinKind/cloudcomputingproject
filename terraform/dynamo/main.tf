provider "aws" {
  region = "ap-southeast-1"
}

module "dynamodb_table" {
  source = "terraform-aws-modules/dynamodb-table/aws"

  name           = "ccproject_devices"
  hash_key       = "id"
  read_capacity  = 2
  write_capacity = 2
  global_secondary_indexes = [
    {
      name               = "deviceIndex"
      hash_key           = "status"
      projection_type    = "INCLUDE"
      non_key_attributes = ["status"]
    }
  ]
  attributes = [
    {
      name = "id"
      type = "S"

    },
    {
      name = "status"
      type = "S"
    }
  ]

  tags = {
    Terraform   = "true"
    Environment = "staging"
  }
}

