require 'thor'
require 'erb'

Attribute = Struct.new(:name, :type) do
  def cpp_type
    case self.type
    when :string
      return "std::string"
    when :int
      return "int"
    when :uint
      return "unsigned int"
    when :boolean
      return "bool"
    end
  end

  def method_type
    case self.type
    when :string
      return :str
    when :int
      return :int
    when :uint
      return :int
    when :boolean
      return :bool
    end
  end
end

Model = Struct.new(:name, :namespace, :attributes)

class ModelGenerator < Thor

  desc "gen MODELNAME ATTRIBUTE:TYPE ...", "Generate a model given attributes and name"
  option "namespace", default: ""
  def gen(name, *attributes)
      @model = Model.new
      @model.name = name
      @model.namespace = options[:namespace]
      @model.attributes = []
      attributes.each do |attr|
        attr_name, type = attr.split(":")
        @model.attributes << Attribute.new(attr_name, type.to_sym)
      end

      header = ERB.new(File.open("model.h.erb","r").read())
      header_out = File.new("#{@model.name}.h", "w")
      header_out.write(header.result(binding))
      header_out.close
      puts "Generated header file: #{@model.name}.h"

      cpp = ERB.new(File.open("model.cpp.erb","r").read())
      cpp_out = File.new("#{@model.name}.cpp", "w")
      cpp_out.write(cpp.result(binding))
      cpp_out.close
      puts "Generated source file: #{@model.name}.cpp"
  end
end

ModelGenerator.start
