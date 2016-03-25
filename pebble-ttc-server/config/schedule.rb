every 1.day, :at => '2:15 am' do
  rake "db:reset"
  rake "ttc:crawl"
end
