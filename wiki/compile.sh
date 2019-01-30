
### the main wiki page
pandoc source/main-page.md -s -H source/style.css -o build/main-page.html

### strategy
pandoc source/vision.md -s -H source/style.css -o build/vision.html
pandoc source/marketing.md -s -H source/style.css -o build/marketing.html
pandoc source/existing-services.md -s -H source/style.css -o build/existing-services.html

### technology
pandoc source/modeling.md -s -H source/style.css -o build/modeling.html
pandoc source/simulation.md -s -H source/style.css -o build/simulation.html
pandoc source/interface.md -s -H source/style.css -o build/interface.html
pandoc source/infrastructure.md -s -H source/style.css -o build/infrastructure.html
pandoc source/evolutions.md -s -H source/style.css -o build/evolutions.html

