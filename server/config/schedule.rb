job_type :rake_without_db_env_check, "cd :path && DISABLE_DATABASE_ENVIRONMENT_CHECK=1 :environment_variable=:environment bundle exec rake :task --silent :output"

set :output, '~/pebble-ttc-cronjobs.log'

every :monday, :at => '2:15 am' do
  rake_without_db_env_check "db:reset ttc:crawl"
end
