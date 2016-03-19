class CreateRoutes < ActiveRecord::Migration[5.0]
  def change
    create_table :routes do |t|
      t.string :tag, index: true
      t.string :title
    end
  end
end
