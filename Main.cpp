#include <iostream>

#include "crow.h"

static crow::json::rvalue readJsonFile(const std::string& file) {
  std::ifstream stream{file};
  std::ostringstream result;
  result << stream.rdbuf();
  stream.close();
  return crow::json::load(result.str());
}

static auto loadSimplePage(const crow::json::rvalue& pages, const std::string& page, crow::mustache::context ctx = {}) {
  ctx["title"] = crow::mustache::compile(pages[page]["title"].s()).render_string(ctx);
  ctx["description"] = crow::mustache::compile(pages[page]["description"].s()).render_string(ctx);
  return crow::mustache::load("simple_page.html").render(ctx);
}

int main() {
  auto problems = readJsonFile("json/problems.json");
  auto variables = readJsonFile("json/variables.json");
  auto pages = readJsonFile("json/pages.json");

  crow::SimpleApp app;

  CROW_ROUTE(app, "/")([&pages]() -> crow::response {
    return loadSimplePage(pages, "welcome");
  });

  CROW_ROUTE(app, "/styles.css")([]() -> crow::response {
    return crow::mustache::load_text("styles.css");
  });

  CROW_ROUTE(app, "/favicon.ico")([](crow::response& res) {
    res.set_static_file_info("favicon.ico");
    res.end();
  });

  CROW_ROUTE(app, "/<uint>/")([&problems, &variables, &pages](size_t index) -> crow::response {
    if (index > problems.size())
      return loadSimplePage(pages, "invalid");
    else if (index == problems.size())
      return loadSimplePage(pages, "congratulations");

    crow::mustache::context ctx{variables};
    ctx["index"] = index;
    ctx["title"] = problems[index]["title"];
    ctx["input"] = problems[index]["input"];
    ctx["description"] = crow::mustache::load("problems/" + std::to_string(index) + ".html").render_string(ctx);

    return crow::mustache::load("problem.html").render(ctx);
  });

  CROW_ROUTE(app, "/<uint>.txt")([&problems](size_t index) -> crow::response {
    if (index > problems.size())
      return crow::response(crow::status::NOT_FOUND);
    return crow::mustache::load("input/" + std::to_string(index) + ".txt").render_string();
  });

  CROW_ROUTE(app, "/<uint>/answer")([&problems, &pages](const crow::request& req, size_t index) -> crow::response {
    if (index > problems.size())
      return crow::response(crow::status::NOT_FOUND);

    crow::mustache::context ctx;

    auto* answer = req.url_params.get("answer");
    if (answer == nullptr || problems[index]["answer"] != answer) {
      ctx["index"] = index;
      return loadSimplePage(pages, "failure", ctx);
    }

    ctx["index"] = index + 1;
    return loadSimplePage(pages, "success", ctx);
  });

  CROW_ROUTE(app, "/all/")([&problems, &pages]() -> crow::response {
    crow::mustache::context ctx;
    for (std::size_t i = 0; i < problems.size(); i++) {
      ctx["problems"][i]["index"] = i;
      ctx["problems"][i]["title"] = problems[i]["title"];
    }
    return loadSimplePage(pages, "all", ctx);
  });

  app.multithreaded().port(18081).run();

  return 0;
}