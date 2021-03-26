#include "pft.hpp"
#include <ROOT/RDataFrame.hxx>
#include <TMath.h>
#include <filesystem>

using RDF    = ROOT::RDataFrame;
namespace fs = std::filesystem;

template <typename... Types>
void print_log(FILE* stream, Types... args)
{
  pft::println(stream, "[LOG] ", args...);
}

void print_usage()
{
  pft::println(stderr, "waveform2csv usage:");
  pft::println(stderr, "If [mode] = 1, save each event in a separate "
                       "file");
  pft::println(stderr, "If [mode] = -a, save all the events in a file");
  pft::println(stderr, "By default save all the events unless a separate file "
                       "is given with the id of events to be saved.");
  pft::println(stderr, "./wave2csv [mode] [filename.root]");
  pft::println(stderr,
               "./wave2csv [mode] [filename.root] [events-to-save.txt]");
}

void print_time_vec(FILE* stream, const Vec<f64>& times)
{
  fprintf(stream, " %zu", times.size());
  for (auto&& t : times) {
    fprintf(stream, " %f", t);
  }
}

auto main(i32 argc, c8* argv[]) -> i32
{
  if (argc < 3) {
    print_usage();
    pft::panic("Wrong Arguments Given");
  }

  auto mode = 0;

  RDF df("t", argv[2]);

  auto df_aux = df.Define("xs",
                          [](TGraph& graph) {
                            auto points  = graph.GetX();
                            const auto n = graph.GetN();
                            return Vec<f64>(points, points + n);
                          },
                          {"Waveform"})
                    .Define("ys",
                            [](TGraph& graph) {
                              auto points  = graph.GetY();
                              const auto n = graph.GetN();
                              return Vec<f64>(points, points + n);
                            },
                            {"Waveform"});

  const auto starting_time =
      df_aux.Take<Vec<f64>>("LightSource_time").GetValue();
  const auto xs = df_aux.Take<Vec<f64>>("xs").GetValue();
  const auto ys = df_aux.Take<Vec<f64>>("ys").GetValue();
  pft::StringView output_filename;

  auto events_to_save = Vec<i32>();
  if (argc == 3) {
    events_to_save = pft::arange<i32>(0, xs.size());
  } else if (argc == 4) {
    pft::println(stdout, "asdf", argv[1]);
    if (!strcmp(argv[1], "-a")) {
      output_filename = argv[3];
      events_to_save  = pft::arange<i32>(0, xs.size());
    } else {
      mode           = pft::to_i32(argv[1]);
      events_to_save = pft::map(pft::to_i32, pft::readlines(argv[3]));
    }
  } else {
    pft::panic("THE HECK DUDE?");
  }
  const auto folder_name = "tmp_wf";
  print_log(stdout, "Creating folder: ", folder_name);
  fs::create_directory(folder_name);
  const fs::path p1 = folder_name;
  fs::current_path(p1);
  FILE* fp = nullptr;
  if (mode == 1) {
    for (const auto& event : events_to_save) {
      const std::string filename = "waveform_" + std::to_string(event) + ".txt";
      pft::println(stdout, "Saving event: ", event);

      fp = fopen(filename.data(), "w");

      for (const auto& [x, y] : pft::zip_to_pair(xs[event], ys[event])) {
        fprintf(fp, "%f %f\n", x, y);
      }
      fclose(fp);
    }
  } else {
    // const std::string filename = output_filename;
    print_log(stdout, "Writing to file: ", output_filename);
    fp = fopen(output_filename.data(), "w");
    for (const auto& event : events_to_save) {
      print_log(stdout, "Saving event: ", event);
      fprintf(fp, "%d %zu", event, xs[event].size());
      print_time_vec(fp, starting_time[event]);
      fprintf(fp, "\n");
      for (const auto& [x, y] : pft::zip_to_pair(xs[event], ys[event])) {
        fprintf(fp, "%f %f\n", x, y);
      }
    }
    fclose(fp);
  }
  return 0;
}
