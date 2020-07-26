from scipy.stats import entropy
import pandas
import numpy
from tqdm import tqdm
import sys
import seaborn
import matplotlib
pandas.set_option('display.max_columns', None)
from tqdm.notebook import trange, tqdm
from time import sleep
import numpy
from joblib import Parallel, delayed
seaborn.set_palette("Paired")
import subprocess
import os.path

ALGORITHM_IQS = 0
ALGORITHM_IIQS = 1
ALGORITHM_RIIQS  = 2
#OUTPUT_IMAGE_PATH="image.png"

def set_output_image_path(image_path):
    global OUTPUT_IMAGE_PATH
    OUTPUT_IMAGE_PATH = image_path
    pass

    
def get_output_image_path():
    return OUTPUT_IMAGE_PATH

def instanciate_runner(executable_path, input_path="./", output_path="/tmp"):

    if os.path.exists(executable_path):
        def execute_experiment( input_file:str, input_size:int, extractions:int, output_file:str = None, generate_dataframe=False, enable_reuse=0,
                                log_pivot_time = 0, log_iteration_time = 0, log_extraction_time = 0, log_swaps = 0, use_random_pivot = 0,
                                use_bfprt = 0, use_iiqs = 0, set_bfprt_alpha = 0.3, set_bfprt_beta = 0.7, set_random_seed = 42, 
                                set_pivot_bias = 0.5, set_redundant_bias = 0.5, thread_id="" ):
            if output_file == None:
                output_file = input_file.split("/")[-1]
            arguments = [
            ('--log-iteration-time=' + str(log_iteration_time)),
            ('--log-pivot-time=' + str(log_iteration_time)),
            ('--log-swaps=' + str(log_iteration_time)),
            ('--log-extraction-time=' + str(log_extraction_time)),
            ('--enable-reuse=' + str(enable_reuse)),
            ('--use-bfprt=' + str(use_bfprt)),
            ('--use-iiqs=' + str(use_iiqs)),
            ('--set-bfprt-alpha=' + str(numpy.round(set_bfprt_alpha,6))),
            ('--set-bfprt-beta=' + str(numpy.round(set_bfprt_beta,6))),
            ('--use-random-pivot=' + str(use_random_pivot)),
            ('--set-random-seed=' + str(set_random_seed)),
            ('--set-pivot-bias=' + str(numpy.round(set_pivot_bias,6))),
            ('--set-redundant-bias=' + str(numpy.round(set_redundant_bias, 6))),
            ('--input-file=' + '/'.join([input_path, str(input_file)])),
            ('--output-file=' + '/'.join([output_path, str(output_file + thread_id)])),
            ('--input-size=' + str(input_size)),
            ('--extractions=' + str(extractions))]
            # print(" ".join([''+executable_path] + arguments))
            result = subprocess.run(" ".join([''+executable_path] + arguments), shell=True,  stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            # print(result.stdout.decode('utf-8'))
            # print(result.stderr.decode('utf-8'))
            result_df = pandas.read_csv('/'.join([output_path, str(output_file + thread_id)]))
            
            result_deletion = subprocess.run(
                ("rm " + '/'.join([output_path, str(output_file + thread_id)]))
                , shell=True,  stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            
            return result_df
        return execute_experiment
    else:
        return None

#runner = instanciate_runner('/home/kuky_nekoi/magicode/program/base_benchmark/main')
runner = instanciate_runner('/home/kuky_nekoi/Documents/Sources/KuKy_NeKoi/magicode/program/base_benchmark/main_prototype',
                            input_path='/home/kuky_nekoi/Documents/Sources/KuKy_NeKoi/magicode/experiments/01_base_benchmark_data')

def characterize_algorithm(x, mapper = {0:"IQS", 1:"IIQS", 2:"bIIQS"}):
    return mapper[x]

def extract_subplot__(axis, main_figure, path):
    extent = axis.get_window_extent().transformed(main_figure.dpi_scale_trans.inverted())
    main_figure.savefig(path + ".png", bbox_inches=extent)
    main_figure.savefig(path + ".expanded.png", bbox_inches=extent.expanded(1.1, 1.1))
    
def extract_subplot(axis, main_figure, path):
    bbox = axis.get_tightbbox(main_figure.canvas.get_renderer())
    main_figure.savefig(path + ".png",
          bbox_inches=bbox.transformed(main_figure.dpi_scale_trans.inverted()).expanded(1.1, 1.1))
    
def segregate_plot(filename, tag, title, plotter, **args):
    original_return = None
    if "ax" in args:
        original_return = plotter(**args)
        if title is not None:
            original_return.set_title(title)
    
    fig, axes = matplotlib.pyplot.subplots(nrows=1, ncols=1, figsize=(8,8))
    
    axes.set(xscale=args["ax"].get_xscale())
    axes.set(yscale=args["ax"].get_yscale())
    args["ax"] = axes
    plot = plotter(**args)
    if title is not None:
        plot.set_title(title)
    fig.savefig(filename + "." + tag + ".png")
    return original_return
    
def plot_base_benchmark(dataframe, output_file, size=(16, 24), prepend="", hue="param.use_iiqs"):
    
    fig, axes = matplotlib.pyplot.subplots(nrows=3, ncols=2, figsize=size)
    destination = get_output_image_path() + output_file
    
    segregate_plot(destination, "0_0", prepend + "Cumulated extraction time", seaborn.lineplot, x="snapshot.current_extraction", y="snapshot.total_extraction_time", 
                    data=dataframe, ax=axes[0,0], hue=hue)
    
    
    segregate_plot(filename=destination, tag="1_0", title=prepend + "Extraction time distribution", plotter=seaborn.scatterplot, x="snapshot.current_extraction", y="snapshot.extraction_time", 
                    data=dataframe, linewidth=0, alpha=0.5, ax=axes[1,0], hue=hue)
    
    
    segregate_plot(filename=destination, tag="2_0", title=prepend + "Stack size per extraction", plotter=seaborn.scatterplot, x="snapshot.current_extraction", y="snapshot.current_stack_size", 
                    data=dataframe, linewidth=0, alpha=0.5, ax=axes[2,0], hue=hue)

    plot_df_head = dataframe[ (dataframe["snapshot.current_extraction"] < 1000)]
    
    #fix for logplot
    #plot_df_head["snapshot.extraction_time"] = plot_df_head["snapshot.extraction_time"].apply(lambda x: x+0.1)
    #plot_df_head["snapshot.current_stack_size"] = plot_df_head["snapshot.current_stack_size"].apply(lambda x: x+0.1)
    #display(plot_df_head["snapshot.current_stack_size"].describe())
    
    axes[0,1].set(yscale="symlog")
    axes[1,1].set(yscale="symlog")
    axes[2,1].set(yscale="symlog")

    segregate_plot(filename=destination, tag="0_1", title=prepend + "Cumulated extraction time", plotter=seaborn.lineplot, x="snapshot.current_extraction", y="snapshot.total_extraction_time", 
                    data=plot_df_head, ax=axes[0,1], hue=hue)
    
    segregate_plot(filename=destination, tag="1_1", title=prepend + "Extraction time distribution", plotter=seaborn.scatterplot, x="snapshot.current_extraction", y="snapshot.extraction_time", 
                    data=plot_df_head, linewidth=0, alpha=0.5, ax=axes[1,1], hue=hue)
    
    segregate_plot(filename=destination, tag="2_1", title=prepend + "Stack size per extraction", plotter=seaborn.scatterplot, x="snapshot.current_extraction", y="snapshot.current_stack_size", 
                    data=plot_df_head, linewidth=0, alpha=0.5, ax=axes[2,1], hue=hue)
    
    fig.savefig(destination)
    
    
def parallel_riiqs(filename, n=10**4, classes=0, noise=0, alpha=0.3, algorithm=2, beta=0.7, extractions=10, tid="", postprocess=None, redundant_bias_param = 1, pivot_bias_param = 0):
    alpha = numpy.round(alpha,7)
    beta = numpy.round(beta,7)
    print("parallel_iiqs(alpha={}, beta={}, filename={}, extractions={})".format(alpha, beta, filename, extractions))
    result_riiqs = runner(filename, n, extractions, 
                        generate_dataframe=True,
                        log_extraction_time=1,
                        set_pivot_bias=pivot_bias_param, use_iiqs=algorithm, 
                        use_bfprt=1, set_redundant_bias=redundant_bias_param,
                        set_bfprt_alpha=alpha, set_bfprt_beta=beta, 
                        thread_id=tid)
    result_riiqs["experiment.classes"] = classes
    result_riiqs["experiment.noise"] = noise
    if postprocess:
        result_riiqs = postprocess(result_riiqs)
    return result_riiqs

def plot_3d_scatter(x,y,z, data, fig, bounds=(1,1,1,), preprocess=None, filters=[], title="",
                    logcolor=True, log_x=False, log_y=False, log_z=False,
                    invert_x=False, invert_y=False, invert_z=False,
                    projection="3d", proj_type = 'persp',
                    view_angle = None, s=100, alpha=1, base_output=None):
    #ax = fig.add_subplot(2, 2, 1, projection='3d')
    ax = fig.add_subplot(bounds[0],bounds[1],bounds[2], projection=projection, proj_type = proj_type)
    
    fig_ =  matplotlib.pyplot.figure(figsize=(8,8))
    ax_ = fig_.add_subplot(1,1,1, projection=projection, proj_type = proj_type)
    
    sample = data
    
    if preprocess:
        sample = preprocess(sample)
    
    for custom_filter in filters:
        if custom_filter[1] == "eq":
            sample = sample[(sample[custom_filter[0]] == custom_filter[2])]  
        elif custom_filter[1] == "leq":
            sample = sample[(sample[custom_filter[0]] <= custom_filter[2])]  
        elif custom_filter[1] == "geq":
            sample = sample[(sample[custom_filter[0]] >= custom_filter[2])]  
        elif custom_filter[1] == "le":
            sample = sample[(sample[custom_filter[0]] < custom_filter[2])]  
        elif custom_filter[1] == "gt":
            sample = sample[(sample[custom_filter[0]] > custom_filter[2])]
    
    _x = sample[x]
    _y = sample[y]
    _z = sample[z]
    _c = _z
    
    if logcolor:
       _c = numpy.log10(_z)
    
    if log_x:
        ax.xaxis.set_scale('log')
        ax_.xaxis.set_scale('log')
    if log_y:
        ax.yaxis.set_scale('log')
        ax_.yaxis.set_scale('log')
    if log_z:
        ax.zaxis.set_scale('log')
        ax_.zaxis.set_scale('log')
        
    if invert_x:
        ax.invert_xaxis()
        ax_.invert_xaxis()
    if invert_y:
        ax.invert_yaxis()
        ax_.invert_yaxis()
    if invert_z:
        ax.invert_zaxis()
        ax_.invert_zaxis()
    
    ax.scatter(_x, _y, _z, c=_c, cmap='rainbow',s=s, alpha=alpha)
    ax.set_xlim(min(_x), max(_x))
    ax.set_ylim(min(_y), max(_y))
    ax.set_zlim(min(_z), max(_z))
    ax.set_xlabel(x)
    ax.set_ylabel(y)
    ax.set_zlabel(z)
    ax.set_title(title)
    
    if view_angle:
        ax.view_init(view_angle[0], view_angle[1])
        
    ax_.scatter(_x, _y, _z, c=_c, cmap='rainbow',s=s, alpha=alpha)
    ax_.set_xlim(min(_x), max(_x))
    ax_.set_ylim(min(_y), max(_y))
    ax_.set_zlim(min(_z), max(_z))
    ax_.set_xlabel(x)
    ax_.set_ylabel(y)
    ax_.set_zlabel(z)
    ax_.set_title(title)
    
    if view_angle:
        ax_.view_init(view_angle[0], view_angle[1])
    
    column = bounds[2] % bounds[1]
    row = int(bounds[2] / bounds[0])
    
    if base_output is not None:
        fig_.savefig(base_output + ".{}_{}.png".format(column, row), bbox_inches='tight')

def plot_base_benchmark_3d(x,y,z, data, filters, title, file, s=100, alpha=1, algorithm_1=1, algorithm_2=2, 
                           algorithm_name_1="IIQS", algorithm_name_2="rIIQS",
                           default_filters_1 = [("param.use_iiqs", "eq", 1), ("snapshot.current_extraction", "eq", 0)],
                           default_filters_2 = [("param.use_iiqs", "eq", 2), ("snapshot.current_extraction", "eq", 1)]):
    fig = matplotlib.pyplot.figure(figsize=(16,24))

    destination = get_output_image_path() + file
    
    plot_3d_scatter(x=x, y=y, z=z, 
                    data=data, fig=fig,
                    bounds=(3,2,1,), preprocess=None, filters=default_filters_1 + filters, 
                    title=algorithm_name_1 + " - " + title, s=s, alpha=alpha, base_output=destination)
    
    plot_3d_scatter(x=x, y=y, z=z, 
                    data=data, fig=fig,
                    bounds=(3,2,2,), preprocess=None, filters=default_filters_2 + filters, 
                    title=algorithm_name_2 + " - " + title, s=s, alpha=alpha, base_output=destination)

    plot_3d_scatter(x=x, y=y, z=z, 
                    data=data, fig=fig,
                    bounds=(3,2,3,), preprocess=None, filters=default_filters_1 + filters, 
                    title=algorithm_name_1 + " - " + title,
                   view_angle=(60, 30), s=s, alpha=alpha, base_output=destination)
    
    plot_3d_scatter(x=x, y=y, z=z, 
                    data=data, fig=fig,
                    bounds=(3,2,4,), preprocess=None, filters=default_filters_2 + filters, 
                    title=algorithm_name_2 + " - " + title,
                   view_angle=(60, 30), s=s, alpha=alpha, base_output=destination)
    
    plot_3d_scatter(x=x, y=y, z=z, 
                    data=data, fig=fig,
                    bounds=(3,2,5,), preprocess=None, filters=default_filters_1 + filters, 
                    title=algorithm_name_1 + " - " + title,
                   view_angle=(10, 30), s=s, alpha=alpha, base_output=destination)

    plot_3d_scatter(x=x, y=y, z=z, 
                    data=data, fig=fig,
                    bounds=(3,2,6,), preprocess=None, filters=default_filters_2 + filters, 
                    title=algorithm_name_2 + " - " + title,
                   view_angle=(10, 30), s=s, alpha=alpha, base_output=destination)

    fig.savefig(destination, bbox_inches='tight')
    
    
    
def log_column(column):
    def _lambda(df):
        df["log("+column+")"] = numpy.log10(df[column])
        return df
    return _lambda
    
    
    
def plot_stack_benchmark_3d(x,y,z, data, filters, title, file, s=100, alpha=1, z2=None):
    fig =  matplotlib.pyplot.figure(figsize=(16,24))
    
    max_extraction = data["snapshot.current_extraction"].max()
    
    if not z2:
        z2 = z

    plot_3d_scatter(x=x, y=y, z=z, 
                    data=data, fig=fig,
                    bounds=(3,2,1,), preprocess=None, filters=[("param.use_iiqs", "eq", 1)] + filters, 
                    title="IIQS - " + title, s=s, alpha=alpha)
    
    plot_3d_scatter(x=x, y=y, z=z2, 
                    data=data, fig=fig,
                    bounds=(3,2,2,), preprocess=None, filters=[("param.use_iiqs", "eq", 1)] + filters, 
                    title="IIQS - " + title, s=s, alpha=alpha)

    plot_3d_scatter(x=x, y=y, z=z, 
                    data=data, fig=fig,
                    bounds=(3,2,3,), preprocess=None, filters=[("param.use_iiqs", "eq", 1)] + filters, 
                    title="IIQS - " + title,
                   view_angle=(60, 30), s=s, alpha=alpha)
    
    plot_3d_scatter(x=x, y=y, z=z2, 
                    data=data, fig=fig,
                    bounds=(3,2,4,), preprocess=None, filters=[("param.use_iiqs", "eq", 1)] + filters, 
                    title="IIQS - " + title,
                   view_angle=(60, 30), s=s, alpha=alpha)
    
    plot_3d_scatter(x=x, y=y, z=z, 
                    data=data, fig=fig,
                    bounds=(3,2,5,), preprocess=None, filters=[("param.use_iiqs", "eq", 1)] + filters, 
                    title="IIQS - " + title,
                   view_angle=(10, 30), s=s, alpha=alpha)

    plot_3d_scatter(x=x, y=y, z=z2, 
                    data=data, fig=fig,
                    bounds=(3,2,6,), preprocess=None, filters=[("param.use_iiqs", "eq", 1)] + filters, 
                    title="IIQS - " + title,
                   view_angle=(10, 30), s=s, alpha=alpha)

    fig.savefig(get_output_image_path() + file, bbox_inches='tight')