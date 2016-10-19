class Route < ApplicationRecord
  has_many :directions

  scope :for_current_time, -> {
    blue_night_time? ? all : where("routes.tag not like '3__'")
  }

  private

  def self.blue_night_time?
    # Blue night runs between 1:30 and 5:00. We'll give it 30 min of leeway
    # (cf.: http://www.ttc.ca/images/fixedImages/TTC-bluenight.pdf)
    now_in_toronto.between?('01:00', '05:30')
  end

  def self.now_in_toronto
    Time.find_zone("America/Toronto").now.strftime('%H:%M')
  end
end
