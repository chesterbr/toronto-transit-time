FROM ruby:2.6

COPY . /usr/src/app/

WORKDIR /usr/src/app
RUN bundle install

CMD ["bin/rails", "server", "--binding", "0.0.0.0"]
