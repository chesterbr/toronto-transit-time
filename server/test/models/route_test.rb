require 'test_helper'

class RouteTest < ActiveSupport::TestCase
  test "#for_current_time supresses 3xx routes before 1:00AM and after 5:30AM" do
    %w(0:00 0:59 5:31 9:00 12:00 15:25 23:39).each do |time|
      hour, minute = time.split(':').map(&:to_i)
      Timecop.travel(today_in_toronto_at(hour, minute)) do
        routes = Route.for_current_time
        assert_equal %w(33 666), routes.map(&:tag).sort,
          "3xx route should have been suppressed at #{time}"
      end
    end
  end

  test "#for_current_time includes 3xx between 1:00AM and 5:30AM" do
    %w(1:00 1:30 4:00 5:00 5:30).each do |time|
      hour, minute = time.split(':').map(&:to_i)
      Timecop.travel(today_in_toronto_at(hour, minute)) do
        routes = Route.for_current_time
        assert_equal %w(315 33 666), routes.map(&:tag).sort,
          "3xx route should have been included at #{time}"
      end
    end
  end

  private

  def today_in_toronto_at(hour, minute)
    Time.find_zone("America/Toronto").local(2016, 1, 1, hour, minute)
  end
end
