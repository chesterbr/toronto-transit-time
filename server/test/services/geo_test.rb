require 'test_helper'

class GeoTest < ActiveSupport::TestCase
  test '#distance_criteria builds an SQL expression that sorts comparably to geographic or cartesian distance' do
    unrooted_distance = '((lat-(11.0))*(lat-(11.0)))+((lon-(22.0))*(lon-(22.0)))'
    assert_equal unrooted_distance, Geo.distance_criteria(11,22)
  end

  test '#square calculates a "square" fence with all sides at a given distance from a given point' do
    # 500m in all four directions from Toronto City Hall (a square with 1KM side on a planified map)
    # View the four corners and center: http://www.darrinward.com/lat-long/?id=1895544
    corner_1_lat, corner_1_lon = 43.6486, -79.3894
    center_lat,   center_lon   = 43.6532, -79.3832
    corner_2_lat, corner_2_lon = 43.6577, -79.3769

    lat_range, lon_range = Geo.square(center_lat, center_lon, 500)

    # 0.0001 deltas will give us good enough precision (order of 10m or so)
    # (https://en.wikipedia.org/wiki/Decimal_degrees)
    assert_in_delta lat_range.begin, corner_1_lat, 0.0001
    assert_in_delta lon_range.begin, corner_1_lon, 0.0001
    assert_in_delta lat_range.end, corner_2_lat, 0.0001
    assert_in_delta lon_range.end, corner_2_lon, 0.0001
  end
end
