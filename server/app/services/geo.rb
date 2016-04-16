module Geo
  def self.distance_criteria(lat, lon)
    d1 = "(lat-(#{lat.to_f}))"
    d2 = "(lon-(#{lon.to_f}))"

    "(#{d1}*#{d1})+(#{d2}*#{d2})"
  end

  def self.square(lat, lon, distance_meters)
    distance = meters_to_earth_degrees(distance_meters)
    delta_lon = distance / (Math.cos(degrees_to_radians(lat))).abs
    delta_lat = distance

    [
      (lat - delta_lat..lat + delta_lat),
      (lon - delta_lon..lon + delta_lon)
    ]
  end

  private

  def self.meters_to_earth_degrees(distance)
    distance * 0.00000900537681
  end

  def self.degrees_to_radians(angle)
    angle * Math::PI / 180
  end
end
