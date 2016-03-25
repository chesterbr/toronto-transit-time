set :output, '~/pebble-ttc-cronjobs.log'

every :monday, :at => '2:15 am' do
  rake "db:reset ttc:crawl"
end
