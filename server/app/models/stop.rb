class Stop < ApplicationRecord
  has_and_belongs_to_many :directions

  scope :within_fence_ordered_by_distance, ->(lat, lon, distance) {
    lat_range, lon_range = Geo.square(lat, lon, distance)
    order_expr = Arel.sql(Geo.distance_criteria(lat,lon))

    where(lat: lat_range, lon: lon_range)
      .order(order_expr)
  }

  scope :with_routes_and_directions_grouped_by_address, -> {
    joins(directions: :route)
      .merge(Route.for_current_time)
      .select('stops.title      stop,     ' \
              'routes.title     route,    ' \
              'directions.title direction,' \
              'routes.tag       r,        ' \
              'stops.tag        s         ')
      .group_by(&:stop)
  }

  class << self
    INITIAL_DISTANCE = 250
    MAX_DISTANCE = 2000

    def as_pebble_menu(lat, lon, stops_count)
      stops_flat = nearest_stops_with_routes(lat, lon, stops_count)
      array_of_stop_and_routes_hashes(stops_flat)
    end

    private

    def nearest_stops_with_routes(lat, lon, stops_count)
      stops = []
      distance = INITIAL_DISTANCE
      loop do
        stops = Stop.within_fence_ordered_by_distance(lat, lon, distance)
                    .with_routes_and_directions_grouped_by_address
                    .to_a
        distance *= 2
        break if stops.count >= stops_count || distance > MAX_DISTANCE
      end
      stops.take(stops_count)
    end

    def array_of_stop_and_routes_hashes(stops)
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
      "https://retro.umoiq.com/service/publicXMLFeed?#{params.to_query}"
    end
  end
end
