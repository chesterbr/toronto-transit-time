class MenusController < ApplicationController
  def show
    lat, lon = params[:lat].to_f, params[:lon].to_f
    lat_range, lon_range = square(lat, lon, 500)

    stops_and_routes =
      Stop
        .where(lat: lat_range, lon: lon_range)
        .joins(directions: :route)
        .order(distance_criteria(lat,lon))
        .select('stops.title      stop,     ' \
                'routes.title     route,    ' \
                'directions.title direction,' \
                'routes.tag       r,        ' \
                'stops.tag        s         ')
        .group_by(&:stop)
        .to_a.map do |stop|
          {
            stop: stop[0],
            routes: stop[1].map { |route| route
                .slice(*%w(route direction))
                .merge({uri: predictions_uri(route)})
            }
          }
        end

    render json: stops_and_routes.to_json
  end

  private

  def distance_criteria(lat, lon)
    d1 = "(lat-(#{lat.to_f}))"
    d2 = "(lon-(#{lon.to_f}))"

    "(#{d1}*#{d1})+(#{d2}*#{d2})"
  end

  def predictions_uri(route)
    params = route.as_json.slice('r', 's').merge({
      command: :predictions,
      a: :ttc
    })
    "http://webservices.nextbus.com/service/publicXMLFeed?#{params.to_query}"
  end

  def square(lat, lon, distance_meters)
    distance = meters_to_earth_degrees(distance_meters)
    delta_lon = distance / (Math.cos(degrees_to_radians(lat))).abs
    delta_lat = distance

    [
      (lat - delta_lat..lat + delta_lat),
      (lon - delta_lon..lon + delta_lon)
    ]
  end

  def meters_to_earth_degrees(distance)
    distance * 0.00000900537681
  end

  def degrees_to_radians(angle)
    angle * Math::PI / 180
  end
end
