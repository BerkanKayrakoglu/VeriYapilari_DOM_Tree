import os
import random

def generate_html_node(tag, depth, max_depth, width):
    if depth > max_depth:
        return ""
    
    # Rastgele id ve sınıf özellikleri üretelim
    id_str = f' id="node_{depth}_{random.randint(1000, 9999)}"' if random.random() > 0.5 else ""
    class_str = f' class="group_{random.choice(["container", "wrapper", "item"])}"' if random.random() > 0.5 else ""
    
    indent = "  " * depth
    html = f"{indent}<{tag}{id_str}{class_str}>\n"
    
    # Çocuk düğümler üret
    if depth < max_depth:
        child_tags = ["div", "p", "span", "section", "article"]
        num_children = random.randint(1, width)
        for _ in range(num_children):
            child_tag = random.choice(child_tags)
            html += generate_html_node(child_tag, depth + 1, max_depth, width)
            
    html += f"{indent}</{tag}>\n"
    return html

def create_synthetic_html(filename, depth, width):
    body_content = generate_html_node("div", 1, depth, width)
    html = f"""<html>
  <head>
    <title>Synthetic Test</title>
  </head>
  <body id="main">
{body_content}  </body>
</html>
"""
    with open(filename, "w", encoding="utf-8") as f:
        f.write(html)
    print(f"Generated {filename} (Depth: {depth}, Max Width: {width}, Filesize: {len(html)} bytes)")

if __name__ == "__main__":
    # datasets klasörünü oluşturalım
    os.makedirs("datasets", exist_ok=True)
    
    # Küçük Veri Seti (Yaklaşık 10-20 düğüm)
    create_synthetic_html("datasets/small.html", depth=3, width=2)
    
    # Orta Veri Seti (Yaklaşık 100-200 düğüm)
    create_synthetic_html("datasets/medium.html", depth=5, width=3)
    
    # Büyük Veri Seti (Yaklaşık 1000+ düğüm)
    create_synthetic_html("datasets/large.html", depth=7, width=4)
