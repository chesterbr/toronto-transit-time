namespace :ttc do
  ROUTE_LIST_URI = 'http://webservices.nextbus.com/service/publicXMLFeed?command=routeList&a=ttc'
  ROUTE_INFO_URI = 'http://webservices.nextbus.com/service/publicXMLFeed?command=routeConfig&a=ttc&r=%{tag}&terse'

  desc 'Crawls the TTC site from official API (http://goo.gl/rb2151)'
  task crawl: :environment do
    puts "[#{Time.now}] Clearing database (just in case - rake db:reset should run before this)"
    wipe_database

    fetch_all_routes_hash.each do |route|
      puts "[#{Time.now}] Importing #{route['title']}..."
      hash = fetch_route_hash(route)

      create_route(hash)
      create_stops(hash)
      create_directions(hash)
      match_direction_stops(hash)

      throttle
    end

    puts "[#{Time.now}] Done. #{Route.count} routes, #{Direction.count} directions and #{Stop.count} stops imported."
  end

  private

  # Fetching from HTTP (XML) API

  def fetch_all_routes_hash
    fetch(ROUTE_LIST_URI)['body']['route']
  end

  def fetch_route_hash(route)
    uri_for_route = ROUTE_INFO_URI % { tag: route['tag'] }
    fetch(uri_for_route)['body']['route']
  end

  # Data transformation/storage

  def wipe_database
    Route.destroy_all
    Direction.destroy_all
    Stop.destroy_all
  end

  def create_route(hash)
    route_hash = hash.slice(*%w(tag title))

    Route.create!(route_hash)
  end

  def create_directions(hash)
    route = Route.find_by(hash.slice('tag'))
    directions_hash = hash['direction'].map { |d| d.slice(*%w(tag title)) }

    name_dedup_regexp = /\b#{route.title.gsub('-','.')} towards /

    parts = route.title.partition('-')
    alternative_id_reduce_regexp = /\b(#{parts[0]}.*) #{parts[2]} towards /

    directions_hash.each do |d|
      d['title'] = d['title']
        .sub(name_dedup_regexp, '')
        .sub(alternative_id_reduce_regexp) { "(#{$1}) " }
        .sub(/\bNorth\b/,'N').sub(/\bSouth\b/,'S').sub(/\bEast\b/,'E').sub(/\bWest\b/,'W')
        .sub(' - ',':')
        .sub(/\btowards\b/, '=>')
        .sub(/\bStation\b/, 'Sta')
    end

    route.directions.create!(directions_hash)
  end

  def create_stops(hash)
    hash['stop'].each do |s|
      s.slice!(*%w(tag title lat lon))
      s['title'] = s['title']
        .gsub(/\b(St|Ave|Rd)\b/,'').gsub('  ',' ')
        .gsub(/\bAt\b/,'@')
        .gsub(/\bNorth\b/,'N').gsub(/\bSouth\b/,'S').gsub(/\bEast\b/,'E').gsub(/\bWest\b/,'W')
        .gsub(/\bPearson Airport\b/,'Pearson')

      Stop.create_with(s)
          .find_or_create_by!(s.slice('tag'))
    end
  end

  def match_direction_stops(hash)
    hash['direction'].each do |d|
      direction = Direction.find_by(d.slice('tag'))
      stop_tags = d['stop'].map{ |s| s['tag'] }

      direction.stops << Stop.where(tag: stop_tags)
    end
  end

  # Utility

  def throttle
    sleep 0.5
  end

  def fetch(uri)
    Hash.from_xml(curl(uri))
  end

  def curl(uri)
    url = URI.parse(uri)
    req = Net::HTTP::Get.new(url.to_s)
    res = Net::HTTP.start(url.host, url.port) do |http|
      http.request(req)
    end
    res.body
  end



end
