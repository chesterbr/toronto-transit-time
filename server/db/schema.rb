# This file is auto-generated from the current state of the database. Instead
# of editing this file, please use the migrations feature of Active Record to
# incrementally modify your database, and then regenerate this schema definition.
#
# Note that this schema.rb definition is the authoritative source for your
# database schema. If you need to create the application database on another
# system, you should be using db:schema:load, not running all the migrations
# from scratch. The latter is a flawed and unsustainable approach (the more migrations
# you'll amass, the slower it'll run and the greater likelihood for issues).
#
# It's strongly recommended that you check this file into your version control system.

ActiveRecord::Schema.define(version: 20160319162500) do

  create_table "directions", force: :cascade do |t|
    t.string  "tag"
    t.string  "title"
    t.integer "route_id"
    t.index ["route_id"], name: "index_directions_on_route_id"
    t.index ["tag"], name: "index_directions_on_tag"
  end

  create_table "directions_stops", id: false, force: :cascade do |t|
    t.integer "direction_id"
    t.integer "stop_id"
    t.index ["direction_id"], name: "index_directions_stops_on_direction_id"
    t.index ["stop_id"], name: "index_directions_stops_on_stop_id"
  end

  create_table "routes", force: :cascade do |t|
    t.string "tag"
    t.string "title"
    t.index ["tag"], name: "index_routes_on_tag"
  end

  create_table "stops", force: :cascade do |t|
    t.string "tag"
    t.string "title"
    t.float  "lat"
    t.float  "lon"
    t.index ["tag"], name: "index_stops_on_tag"
  end

end
