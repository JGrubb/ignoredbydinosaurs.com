module Jekyll
  class MetaDesc <  Liquid::Tag
    
    def initialize(text, tokens)
      super
      if meta_desc
        @text = meta_desc
      else
        @text = content.truncate(250)
      end
    end
    
    def render(context)
      "#{@text}"
    end
  end
end

Liquid::Template.register_tag('meta_desc', Jekyll::MetaDesc)