class Stop < ApplicationRecord
  has_and_belongs_to_many :directions

  scope :within_fence_ordered_by_distance, ->(lat, lon, distance) {
    lat_range, lon_range = Geo.square(lat, lon, distance)
    order_expr = Geo.distance_criteria(lat,lon)

    where(lat: lat_range, lon: lon_range)
      .order(order_expr)
  }

  scope :with_routes_and_directions_grouped_by_address, -> {
    joins(directions: :route)
      .select('stops.title      stop,     ' \
              'routes.title     route,    ' \
              'directions.title direction,' \
              'routes.tag       r,        ' \
              'stops.tag        s         ')
      .group_by(&:stop)
  }

  class << self
    MAX_DISTANCE = 500
    INCREMENT = 100

    def as_pebble_menu(lat, lon, min_stops)
      stops_flat = nearest_stops_with_routes(lat, lon, min_stops)
      hashes_with_stop_and_routes(stops_flat)
    end

    private

    def nearest_stops_with_routes(lat, lon, min_stops)
      stops = []
      distance = 100
      loop do
        stops = Stop.within_fence_ordered_by_distance(lat, lon, distance)
                    .with_routes_and_directions_grouped_by_address
                    .to_a
        distance += INCREMENT
        break if stops.count >= min_stops || distance > MAX_DISTANCE
      end
      stops
    end

    def hashes_with_stop_and_routes(stops)
      stops.map do |stop|
        {
          stop: stop[0],
          routes: stop[1].map { |route| route
              .slice(*%w(route direction))
              .merge({uri: predictions_uri(route)})
          }
        }
      end
    end

    def predictions_uri(route)
      params = route.as_json.slice('r', 's').merge({
        command: :predictions,
        a: :ttc
      })
      "http://webservices.nextbus.com/service/publicXMLFeed?#{params.to_query}"
    end
  end
end
