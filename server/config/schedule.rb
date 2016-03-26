RAKE_CRAWLER_ENV =
  "DISABLE_DATABASE_ENVIRONMENT_CHECK=1 " \
  ":environment_variable=:environment"

job_type :rake_without_db_env_check,
  "cd :path && " \
  "#{RAKE_CRAWLER_ENV} bundle exec rake :task --silent :output && " \
  "sudo supervisorctl restart pebblettc"

set :output, '~/pebble-ttc-cronjobs.log'

every :monday, :at => '2:15 am' do
  rake_without_db_env_check "db:reset ttc:crawl"
end
