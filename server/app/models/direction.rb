class Direction < ApplicationRecord
  belongs_to :route
  has_and_belongs_to_many :stops
end
