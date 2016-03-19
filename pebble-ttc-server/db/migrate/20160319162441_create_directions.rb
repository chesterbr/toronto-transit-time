class CreateDirections < ActiveRecord::Migration[5.0]
  def change
    create_table :directions do |t|
      t.string :tag, index: true
      t.string :title
      t.references :route, foreign_key: true
    end
  end
end
