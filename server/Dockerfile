FROM ruby:2.6

LABEL maintainer="cd@pobox.com"

# Copy Gemfile first so we only bust this cache if gems change
COPY Gemfile* /usr/src/app/
WORKDIR /usr/src/app
RUN bundle install

# Copy the rest of the app
COPY . /usr/src/app/

# Start the server
CMD ["bin/rails", "server", "--binding", "0.0.0.0"]
