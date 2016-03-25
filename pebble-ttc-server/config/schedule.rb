set :output, '/var/log/pebble-ttc-cronjobs.log'

every 1.day, :at => '2:15 am' do
  rake "db:reset ttc:crawl"
end
