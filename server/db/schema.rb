# This file is auto-generated from the current state of the database. Instead
# of editing this file, please use the migrations feature of Active Record to
# incrementally modify your database, and then regenerate this schema definition.
#
# This file is the source Rails uses to define your schema when running `bin/rails
# db:schema:load`. When creating a new database, `bin/rails db:schema:load` tends to
# be faster and is potentially less error prone than running all of your
# migrations from scratch. Old migrations may fail to apply correctly if those
# migrations use external dependencies or application code.
#
# It's strongly recommended that you check this file into your version control system.

ActiveRecord::Schema[7.0].define(version: 2023_05_31_144439) do
  create_table "directions", force: :cascade do |t|
    t.string "tag"
    t.string "title"
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
    t.float "lat"
    t.float "lon"
    t.index ["tag"], name: "index_stops_on_tag"
  end

  add_foreign_key "directions", "routes"
end
