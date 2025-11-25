import argparse, pathlib, os, sys, yaml, jinja2



# 1. directory that holds *this* script
SCRIPT_DIR = pathlib.Path(__file__).resolve().parent

# 2. repo root is one level up
REPO_ROOT  = SCRIPT_DIR.parent



def load_yaml(file):
	with open(file, 'r') as f:
		return yaml.safe_load(f)

def render(tpl_path, ctx):
	tpl_full = SCRIPT_DIR / tpl_path
	return jinja2.Template(tpl_full.read_text()).render(**ctx)

def generate_settings(yaml_file: pathlib.Path):
	cfg      = load_yaml(yaml_file)
	proj     = cfg['project']
	settings = cfg['settings']

	ctx = {'project': proj, 'settings': settings}
	# out_dir  = pathlib.Path(args.out_dir)
	out_dir = pathlib.Path(REPO_ROOT / f'main')
	out_dir.mkdir(exist_ok=True)


	hpp_dir = out_dir / f'include/projects/{proj}'
	cpp_dir = out_dir / f'projects/{proj}'

	hpp_dir.mkdir(exist_ok=True)
	cpp_dir.mkdir(exist_ok=True)

	hpp = hpp_dir / f'fb_{proj}_settings.hpp'
	cpp = cpp_dir / f'fb_{proj}_settings.cpp'

	hpp.write_text(render(pathlib.Path('templates/settings.hpp.j2'), ctx))
	cpp.write_text(render(pathlib.Path('templates/settings.cpp.j2'), ctx))
	print(f'generated {hpp}  {cpp}')

def generate_project(yaml_file: pathlib.Path):
	cfg      = load_yaml(yaml_file)
	proj     = cfg['project']
	settings = cfg['settings']

	ctx = {'project': proj, 'settings': settings, 'name' : cfg['name'], 'version' : cfg['version']}

	out_dir = pathlib.Path(REPO_ROOT / f'main')
	out_dir.mkdir(exist_ok=True)

	hpp_dir = out_dir / f'include/projects/{proj}'
	cpp_dir = out_dir / f'projects/{proj}'

	hpp_dir.mkdir(exist_ok=True)
	cpp_dir.mkdir(exist_ok=True)

	hpp_files = [f"fb_{proj}_box_obj.hpp", f"fb_{proj}_hw_obj.hpp", f"fb_{proj}_mapper_obj.hpp", f"fb_{proj}_pins.hpp"]
	cpp_files = [f"fb_{proj}_box_obj.cpp", f"fb_{proj}_hw_obj.cpp", f"fb_{proj}_mapper_obj.cpp", f"fb_{proj}_core.cpp"]

	hpp = hpp_dir / hpp_files[0]
	hpp.write_text(render(pathlib.Path("templates/project/box.hpp.j2"), ctx))
	hpp = hpp_dir / hpp_files[1]
	hpp.write_text(render(pathlib.Path("templates/project/hw.hpp.j2"), ctx))
	hpp = hpp_dir / hpp_files[2]
	hpp.write_text(render(pathlib.Path("templates/project/mapper.hpp.j2"), ctx))
	hpp = hpp_dir / hpp_files[3]
	hpp.write_text(render(pathlib.Path("templates/project/pins.hpp.j2"), ctx))

	cpp = cpp_dir / cpp_files[0]
	cpp.write_text(render(pathlib.Path("templates/project/box.cpp.j2"), ctx))
	cpp = cpp_dir / cpp_files[1]
	cpp.write_text(render(pathlib.Path("templates/project/hw.cpp.j2"), ctx))
	cpp = cpp_dir / cpp_files[2]
	cpp.write_text(render(pathlib.Path("templates/project/mapper.cpp.j2"), ctx))
	cpp = cpp_dir / cpp_files[3]
	cpp.write_text(render(pathlib.Path("templates/project/core.cpp.j2"), ctx))

	print(f'generated {hpp_files}  {cpp_files}')



def main():
	ap = argparse.ArgumentParser(description='Code generator for FlowerBox projects')
	group = ap.add_mutually_exclusive_group(required=True)
	group.add_argument('-s', '--settings', action='store_true',
					   help='generate only settings files')
	group.add_argument('-p', '--project',  action='store_true',
					   help='generate whole project tree')
	ap.add_argument('yaml', type=pathlib.Path,
                    help='path to project.yml')

	args = ap.parse_args()

	yaml_path = pathlib.Path(args.yaml)
	if not yaml_path.is_file():
		sys.exit(f'YAML not found: {yaml_path}')

	if args.settings:
		generate_settings(yaml_path)
	elif args.project:
		generate_project(yaml_path)



if __name__ == '__main__':
	main()