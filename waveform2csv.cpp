#include "../../pft/pft.hpp"
#include <ROOT/RDataFrame.hxx>
#include <TMath.h>
#include <filesystem>

using RDF    = ROOT::RDataFrame;
namespace fs = std::filesystem;

template <typename... Types>
void print_log(FILE* stream, Types... args) {
  pft::println(stream, "[LOG] ", args...);
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    fprintf(stderr, "NO FILE GIVEN\n");
    exit(1);
  }

  auto mode = pft::to_int(argv[1]);

  RDF df("t", argv[2]);

  auto df_aux = df.Define("xs",
                          [](TGraph& graph) {
                            auto points  = graph.GetX();
                            const auto n = graph.GetN();
                            return std::vector<f32>(points, points + n);
                          },
                          {"Waveform"})
                    .Define("ys",
                            [](TGraph& graph) {
                              auto points  = graph.GetY();
                              const auto n = graph.GetN();
                              return std::vector<f32>(points, points + n);
                            },
                            {"Waveform"});

  const auto xs = df_aux.Take<std::vector<f32>>("xs").GetValue();
  const auto ys = df_aux.Take<std::vector<f32>>("ys").GetValue();

  auto events_to_save = std::vector<i32>();
  if (argc == 3) {
    events_to_save = pft::arange<i32>(0, xs.size());
  } else if (argc == 4) {
    events_to_save = pft::map(pft::to_int, pft::readlines(argv[3]));
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
    const std::string filename = "waveforms.txt";
    fp                         = fopen(filename.data(), "w");
    for (const auto& event : events_to_save) {
      print_log(stdout, "Saving event: ", event);
      fprintf(fp, "%d %zu\n", event, xs[event].size());
      for (const auto& [x, y] : pft::zip_to_pair(xs[event], ys[event])) {
        fprintf(fp, "%f %f\n", x, y);
      }
    }
    fclose(fp);
  }
  return 0;
}
